#ifndef SFTPTRANSFER_H
#define SFTPTRANSFER_H

#include <QObject>
#include <QThread>

#include "core/sshsettings.h"

class SFtpSession;
class SftpTransferInner;
class SftpTransfer : public QObject
{
    Q_OBJECT
public:
    explicit SftpTransfer(SSHSettings const& settings,
                          QObject *parent = nullptr);
    ~SftpTransfer();

    void upload(QString const& srcFileName, QString const& dstfileName);
    void download(QString const& srcFileName, QString const& dstfileName);
    void stop();
signals:
    void startUpload(QString const& srcFileName, QString const& dstfileName);
    void startDownload(QString const& srcFileName, QString const& dstfileName);

    void gotFileSize(quint64 filesize);
    void progressInfo(quint32 blockNumber, quint64 bytesOfSend);
    void finished();
    void error(QString const& e);
private:
   QThread workerThread;
   SftpTransferInner* worker_;
};

class SftpTransferInner : public QObject
{
    Q_OBJECT
public:
    explicit SftpTransferInner(SSHSettings const& settings,
                          QObject *parent = nullptr);
    enum Mode{ NONE, UPLOAD, DOWNLOAD };

    void stop();

public slots:
    void upload(QString const& srcFileName, QString const& dstfileName);
    void download(QString const& srcFileName, QString const& dstfileName);

signals:
    void gotFileSize(quint64 filesize);
    void progressInfo(quint32 blockNumber, quint64 bytesOfSend);
    void finished();
    void error(QString const& e);

private slots:
    void connected();
    void connectionError(QString const& e);
private:
    bool uploadFile();
    bool downloadFile();
    bool singled() { return signal_; }
    void doSignal() { signal_ = true; };
    qint64 getFileSize(QString const& fileName);
private:
    SFtpSession* sftp;
    SSHSettings settings_;
    Mode mode_;
    QString srcFileName_;
    QString dstfileName_;
    volatile bool signal_;
};

#endif // SFTPTRANSFER_H
