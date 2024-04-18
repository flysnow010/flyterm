#ifndef XYMODEMRECVFILE_H
#define XYMODEMRECVFILE_H
#include "ymodem.h"

#include <QObject>

class QSerialPort;
class XYModemRecvFile : public QObject, public YModem
{
    Q_OBJECT
public:
    explicit XYModemRecvFile(QSerialPort *serial, QObject *parent = nullptr);

    enum {
        START_COUNT = 10,
        TRY_COUNT = 5,
        TIME_OUT_MS = 2000
    };
public slots:
    void startYModem(QString const& fileName);
    void startXModem(QString const& fileName);
    void stop();
    void cancel();
signals:
    void gotFileSize(quint64 filesize);
    void progressInfo(quint32 blockNumber, quint64 bytesOfSend);
    void error(QString const& e);
    void finished();

protected:
    uint32_t write(uint8_t const *data, uint32_t size) override;
    uint32_t read(uint8_t *data, uint32_t size) override;
    uint8_t get_code() override;
private:
    bool singled() { return signal_; }
    void doSignal() { signal_ = true; };
private:
     QSerialPort* serial_;
     volatile bool signal_;
     uint8_t data_[DATA_SIZE2];
};

#endif // XYMODEMRECVFILE_H
