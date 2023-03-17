#ifndef XYMODEMSENDFILE_H
#define XYMODEMSENDFILE_H
#include "ymoderm.h"

#include <QObject>

class QSerialPort;

class XYModemSendFile : public QObject, public YModerm
{
    Q_OBJECT
public:
    explicit XYModemSendFile(QSerialPort *serial, QObject *parent = nullptr);

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
    void doError(QString const& text);
private:
     QSerialPort* serial_;
     volatile bool signal_;
};

#endif // XYMODEMSENDFILE_H
