#ifndef KERMITFILESENDER_H
#define KERMITFILESENDER_H

#include <QObject>
#include <QThread>

class QSerialPort;
class KermitSendFile;
class KermitFileSender : public QObject
{
    Q_OBJECT
public:
    explicit KermitFileSender(QSerialPort *serial, QObject *parent = nullptr);
    ~KermitFileSender();

signals:
    void start_send(QString const& fileName);
    void cancel_send();

    void gotFileSize(quint64 filesize);
    void progressInfo(quint32 blockNumber, quint64 bytesOfSend);
    void finished();
    void error(QString const& e);
public:
    void start(QString const& fileName);
    void stop();
    void cancel();
private:
   QThread workerThread;
   KermitSendFile* worker_;
};

#endif // KERMITFILESENDER_H
