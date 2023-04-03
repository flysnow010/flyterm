#ifndef KERMITSENDFILE_H
#define KERMITSENDFILE_H
#include "kermit.h"

#include <QObject>
#include <QFile>

class QSerialPort;
class KermitSendFile : public QObject, public Kermit
{
    Q_OBJECT
public:
    explicit KermitSendFile(QSerialPort *serial, QObject *parent = nullptr);

    enum State {
        SSNUL = 0,
        SSINT = 1,
        SSFIL = 2,
        SSDAT = 3,
        SSEND = 4,
        SSBRK = 5
    };

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

    void on_ack(int seq, const char* data, int size) override;
    void on_nack(int seq, const char* data, int size) override;
    void on_error(int seq, const char* data, int size) override;
private:
    void send_data(int seq);
    int next_seq(int seq) { return (seq + 1) % 64; }
    bool singled() { return signal_; }
    void doSignal() { signal_ = true; };
    bool getchar(char & ch);
private:
     QSerialPort* serial_;
     volatile bool signal_;
     State state_ = SSNUL;
     QFile file_;
     quint32 blockNumber = 0;
     quint64 bytesOfSend_ = 0;
};

#endif // KERMITSENDFILE_H
