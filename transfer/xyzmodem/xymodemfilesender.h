#ifndef XYMODEMFILESENDER_H
#define XYMODEMFILESENDER_H

#include <QObject>
#include <QThread>

class QSerialPort;
class XYModemSendFile;
class XYModemFileSender : public QObject
{
    Q_OBJECT
public:
    explicit XYModemFileSender(QSerialPort *serial, bool isYModem = true,
                              QObject *parent = nullptr);
    ~XYModemFileSender();

signals:
    void start_send(QString const& fileName);
    void stop_send();
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
   XYModemSendFile* worker_;
};

#endif // XYMODEMFILESENDER_H
