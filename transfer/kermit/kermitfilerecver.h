#ifndef KERMITFILERECVER_H
#define KERMITFILERECVER_H

#include <QObject>
#include <QThread>

class QSerialPort;
class KermitFileRecver : public QObject
{
    Q_OBJECT
public:
    explicit KermitFileRecver(QSerialPort *serial, QObject *parent = nullptr);
    ~KermitFileRecver();
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

#endif // KERMITFILERECVER_H
