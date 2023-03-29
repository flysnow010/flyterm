#ifndef XYMODEMFILERECVER_H
#define XYMODEMFILERECVER_H

#include <QObject>
#include <QThread>

class QSerialPort;
class XYModemFileRecver : public QObject
{
    Q_OBJECT
public:
    explicit XYModemFileRecver(QSerialPort *serial, bool isYModem = true,
                               QObject *parent = nullptr);
    ~XYModemFileRecver();

signals:
    void start_recv(QString const& fileName);
    void stop_recv();
    void cancel_recv();

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
};

#endif // XYMODEMFILERECVER_H
