#ifndef KERMITRECVFILE_H
#define KERMITRECVFILE_H
#include "kermit.h"

#include <QObject>
#include <QFile>

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
    int write(char const *data, int size) override;
    int read(char *data, int size) override;
    char getc() override;

    void on_init(int seq, const char* data, int size) override;
    void on_data(int seq, const char* data, int size) override;
    void on_break(int seq, const char* data, int size) override;
private:
    void saveData(const char* data, int size);
    bool singled() { return signal_; }
    void doSignal() { signal_ = true; };
private:
     QSerialPort* serial_;
     volatile bool signal_;
     State state_ = SSNUL;
     QFile file_;
     int timeMs = 0;
     quint32 blockNumber = 0;
     quint64 bytesOfRecv_ = 0;

};

#endif // KERMITRECVFILE_H
