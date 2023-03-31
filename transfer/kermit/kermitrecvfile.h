#ifndef KERMITRECVFILE_H
#define KERMITRECVFILE_H
#include "kermit.h"

#include <QObject>

class QSerialPort;
class KermitRecvFile : public QObject, public Kermit
{
    Q_OBJECT
public:
    explicit KermitRecvFile(QSerialPort *serial, QObject *parent = nullptr);

public slots:
    void start(QString const& fileName);
    void stop();
    void cancel();

signals:
    void gotFileSize(quint64 filesize);
    void progressInfo(quint32 blockNumber, quint64 bytesOfSend);
    void error(QString const& e);
    void finished();
protected:
    uint32_t write(char const *data, uint32_t size) override;
    uint32_t read(char *data, uint32_t size) override;
    char getch() override;
private:
    bool singled() { return signal_; }
    void doSignal() { signal_ = true; };
private:
     QSerialPort* serial_;
     volatile bool signal_;

};

#endif // KERMITRECVFILE_H
