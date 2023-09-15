#include "sftptransfer.h"
#include "sftpsession.h"
#include "ssh/fileinfo.h"

#include <QFile>
#include <QFileInfo>

int const UPLOAD_BUF_SIZE   = 0x19000;//50K
int const DOWNLOAD_BUF_SIZE = 0x19000;//50K

SftpTransfer::SftpTransfer(SSHSettings const& settings, QObject *parent)
    : QObject(parent)
{
    SftpTransferInner* worker = new SftpTransferInner(settings);
    worker->moveToThread(&workerThread);

    connect(&workerThread, &QThread::finished, worker, &SftpTransferInner::deleteLater);
    connect(this, &SftpTransfer::startUpload, worker, &SftpTransferInner::upload);
    connect(this, &SftpTransfer::startDownload, worker, &SftpTransferInner::download);
    connect(worker, &SftpTransferInner::gotFileSize, this, &SftpTransfer::gotFileSize);
    connect(worker, &SftpTransferInner::progressInfo, this, &SftpTransfer::progressInfo);
    connect(worker, &SftpTransferInner::finished, this, &SftpTransfer::finished);
    connect(worker, &SftpTransferInner::error, this, &SftpTransfer::error);

    worker_= worker;
    workerThread.start();
}

SftpTransfer::~SftpTransfer()
{
    workerThread.quit();
    workerThread.wait();
}

void SftpTransfer::upload(QString const& srcFileName, QString const& dstfileName)
{
    emit startUpload(srcFileName, dstfileName);
}

void SftpTransfer::download(QString const& srcFileName, QString const& dstfileName)
{
    emit startDownload(srcFileName, dstfileName);
}

void SftpTransfer::stop()
{
    worker_->stop();
}

SftpTransferInner::SftpTransferInner(SSHSettings const& settings,
                           QObject *parent)
    : QObject(parent)
    , sftp(new SFtpSession(this))
    , settings_(settings)
    , mode_(NONE)
    , signal_(false)
{
    connect(sftp, &SFtpSession::connected, this, &SftpTransferInner::connected);
}

void SftpTransferInner::upload(QString const& srcFileName, QString const& dstfileName)
{
    mode_ = UPLOAD;
    srcFileName_ = srcFileName;
    dstfileName_ = dstfileName;
    sftp->start(settings_);
}

void SftpTransferInner::download(QString const& srcFileName, QString const& dstfileName)
{
    mode_ = DOWNLOAD;
    srcFileName_ = srcFileName;
    dstfileName_ = dstfileName;
    sftp->start(settings_);
}

void SftpTransferInner::stop()
{
    doSignal();
    sftp->stop();
}

void SftpTransferInner::connected()
{
    if(mode_ == UPLOAD)
        uploadFile();
    else if(mode_ == DOWNLOAD)
        downloadFile();
}

bool SftpTransferInner::uploadFile()
{
    QFile file(srcFileName_);
    if(!file.open(QIODevice::ReadOnly))
        return false;

    qint64 filesize = file.size();
    ssh::File::Ptr remotefile = sftp->openForWrite(dstfileName_.toStdString().c_str(),
                                                          filesize);
    if(!remotefile)
        return false;

    qint64 writedsize = 0;
     quint32 blockNumber = 1;
    int percent = 0;
    QByteArray buf(UPLOAD_BUF_SIZE, Qt::Uninitialized);

    emit gotFileSize(filesize);
    while(writedsize < filesize && !singled())
    {
        qint64 size = file.read(buf.data(), buf.size());
        if(size <= 0)
            break;
        ssize_t write_size = remotefile->write(buf.data(), size);
        if(write_size != size)
            break;
        blockNumber++;
        writedsize += size;
        int newpercent = writedsize * 100 / filesize;
        if(newpercent != percent)
        {
            percent = newpercent;
            emit progressInfo(blockNumber, writedsize);
        }
    }
    if(writedsize != filesize)
    {
        emit error(tr("Upload is failed"));
        emit finished();
        return false;
    }

    emit finished();
    return true;
}

bool SftpTransferInner::downloadFile()
{
    ssh::File::Ptr remotefile = sftp->openForRead(srcFileName_.toStdString().c_str());
    if(!remotefile)
        return false;

    QFile file(dstfileName_);
    if(!file.open(QIODevice::WriteOnly))
        return false;

    qint64 filesize = getFileSize(srcFileName_);
    qint64 writedsize = 0;
    quint32 blockNumber = 1;
    int percent = 0;
    QByteArray buf(DOWNLOAD_BUF_SIZE, Qt::Uninitialized);

    emit gotFileSize(filesize);
    while(writedsize < filesize && !singled())
    {
        ssize_t size = remotefile->read(buf.data(), buf.size());
        if(size <= 0)
            break;
        file.write(buf.data(), size);
        blockNumber++;
        writedsize += size;
        int newpercent = writedsize * 100 / filesize;
        if(newpercent != percent)
        {
            percent = newpercent;
            emit progressInfo(blockNumber, writedsize);
        }
    }
    if(writedsize != filesize)
    {
        emit error(tr("Donwload is failed"));
        emit finished();
        return false;
    }

    emit finished();
    return true;
}

qint64 SftpTransferInner::getFileSize(QString const& fileName)
{
    QFileInfo fileInfo(fileName);
    ssh::DirPtr dir = sftp->dir(fileInfo.path().toStdString());
    if(!dir)
        return false;

    ssh::FileInfos fileInfos = dir->fileinfos();
    ssh::FileInfoPtr fileInfoPtr = fileInfos.find(fileInfo.fileName().toStdString());
    if(!fileInfoPtr)
        return 0;

    return  fileInfoPtr->size();
}
