#include "sftpsession.h"
#include "ssh/channel.h"
#include "ssh/dir.h"
#include "sshconnect.h"
#include <fcntl.h>

SFtpSession::SFtpSession(QObject *parent)
    : QObject(parent)
    , sessioin_(0)
{
    qRegisterMetaType<SSHSettings>("SSHSettings");

    SshConnect* worker = new SshConnect();
    worker->moveToThread(&sessionThread);
    connect(&sessionThread, &QThread::finished, worker, &QObject::deleteLater);

    connect(this, &SFtpSession::startSession, worker, &SshConnect::start);
    connect(this, &SFtpSession::stopSession, worker, &SshConnect::stop);

    connect(worker, &SshConnect::connected, this, &SFtpSession::onConnected);
    connect(worker, &SshConnect::unconnected, this, &SFtpSession::unconnected);
    connect(worker, &SshConnect::connectionError, this, &SFtpSession::connectionError);

    sessioin_= worker;

    sessionThread.start();
}

SFtpSession::~SFtpSession()
{
    if(sftp)
        sftp.reset();
    if(scp)
        scp.reset();
    stop();
    sessionThread.quit();
    sessionThread.wait();
}

void SFtpSession::start(SSHSettings const& settings)
{
    emit startSession(settings);
}

void SFtpSession::stop()
{
    //emit stopSession();
    sessioin_->stop();
}

void SFtpSession::onConnected()
{
    ssh::Session::Ptr sessioin = sessioin_->sessioin();
    if(!sftp)
        sftp = ssh::SFtp::Ptr(new ssh::SFtp(*sessioin));
    if(!sftp->init())
    {
        sftp.reset();
        scp = ssh::Scp::Ptr(new ssh::Scp(*sessioin));
    }
    emit connected();
}

ssh::File::Ptr SFtpSession::openForRead(const char* filename)
{
    ssh::File::Ptr file = createFile();
    if(file->open(filename, O_RDONLY, 0644))
        return file;
    return ssh::File::Ptr();
}

ssh::File::Ptr SFtpSession::openForWrite(const char* filename, uint64_t filesize)
{
    ssh::File::Ptr file = createFile();
    file->set_filesize(filesize);
    if(file->open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644))
        return file;
    return ssh::File::Ptr();
}

ssh::File::Ptr SFtpSession::createFile()
{
    ssh::Session::Ptr sessioin = sessioin_->sessioin();
    if(sftp)
        return ssh::File::Ptr(new ssh::File(*sftp));
    else
        return ssh::File::Ptr(new ssh::File(*sessioin));
}

bool SFtpSession::createDir(std::string const& path)
{
    if(!scp)
        return false;

    std::string dirname = ssh::Dir::dirname(path.c_str());
    std::string basename = ssh::Dir::basename(path.c_str());
    if(scp->open(dirname.c_str(), false))
    {
        scp->mkdir(basename.c_str());
        scp->close();
        return true;
    }
    return false;
}

std::string SFtpSession::homeDir()
{
    if(!homedir_.empty())
        return homedir_;
    ssh::Session::Ptr sessioin = sessioin_->sessioin();
    ssh::Channel channel(*sessioin);
    if(!channel.open()
        || !channel.exec("pwd")
        || !channel.poll(1000))
        return homedir_;

    char buf[512];

    int size = channel.read_nonblocking(buf, sizeof(buf));
    if(size > 0)
        homedir_ = std::string(buf, size -1);

    return homedir_;
}

std::string SFtpSession::error() const
{
    return sftp->error();
}
