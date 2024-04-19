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

public slots:
    void startYModem(QString const& fileName);
    void startXModem(QString const& fileName);
    void stop();
    void cancel();
signals:
    void gotFileSize(quint64 filesize);
    void progressInfo(quint32 blockNumber, quint64 bytesOfRecv);
    void error(QString const& e);
    void finished();

protected:
    uint32_t write(uint8_t const *data, uint32_t size) override;
    uint32_t read(uint8_t *data, uint32_t size) override;
    uint8_t get_code(bool isWait = true) override;
    uint32_t do_recv(uint8_t code);
private:
    bool singled() { return signal_; }
    void doSignal() { signal_ = true; };
    void doError(QString const& text);
    const char* data() { return reinterpret_cast<const char*>(data_ + ID); }
    uint64_t get_filesize(const char* data, uint32_t size);
private:
     QSerialPort* serial_;
     volatile bool signal_;
     uint8_t data_[DATA_SIZE2];
};

#endif // XYMODEMRECVFILE_H
