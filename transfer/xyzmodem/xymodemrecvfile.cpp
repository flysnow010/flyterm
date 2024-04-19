#include "xymodemrecvfile.h"
#include <QSerialPort>
#include <QDebug>
#include <QFile>
#include <QApplication>

XYModemRecvFile::XYModemRecvFile(QSerialPort *serial, QObject *parent)
    : QObject(parent)
    , serial_(serial)
    , signal_(false)
{
}

void XYModemRecvFile::startYModem(QString const& fileName)
{
    uint8_t code = wait_start();
    if(code == MAX)
    {
        doError("cannot get start code!");
        return;
    }

    uint32_t size = do_recv(code);
    if(size == 0)
    {
        doError("cannot get start data!");
        return;
    }
    uint64_t fileSize = get_filesize(data(), size);
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
    {
        doError(QString("%1 cannot be opened!").arg(fileName));
        return;
    }

    emit gotFileSize(fileSize);
    quint32 blockNumber = 0;
    uint64_t bytesOfRecv = 0;
    while(!singled())
    {
        code = get_code();
        if(code == EOT)
        {
            tx_code(ACK);
            break;
        }

        size = do_recv(code);
        if(size == 0)
            tx_code(NAK);
        else
        {
            if(file.write(data(), size) != size)
            {
                doError("write file error!");
                return;
            }
            blockNumber++;
            bytesOfRecv += size;
            tx_code(ACK);
            emit progressInfo(blockNumber, bytesOfRecv);
        }
    }

    emit finished();
    serial_->moveToThread(QApplication::instance()->thread());
}

void XYModemRecvFile::startXModem(QString const& fileName)
{
    Q_UNUSED(fileName)
    uint8_t code = wait_start();
    if(code == MAX)
    {
        doError("cannot get start code!");
        return;
    }
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
    {
        doError(QString("%1 cannot be opened!").arg(fileName));
        return;
    }
    quint32 blockNumber = 0;
    uint64_t bytesOfRecv = 0;
    do
    {
        uint32_t size = do_recv(code);
        if(size == 0)
            tx_code(NAK);
        else
        {
            if(file.write(data(), size) != size)
            {
                doError("write file error!");
                return;
            }
            tx_code(ACK);
            blockNumber++;
            bytesOfRecv += size;
            emit progressInfo(blockNumber, bytesOfRecv);
        }
        code = get_code();
        if(code == EOT)
        {
            tx_code(ACK);
            break;
        }
    }while(!singled());

    emit finished();
    serial_->moveToThread(QApplication::instance()->thread());
}

void XYModemRecvFile::stop() { doSignal(); }

void XYModemRecvFile::cancel()
{
    ;
}

uint32_t XYModemRecvFile::do_recv(uint8_t code)
{
    uint32_t size = 0;
    if(code == SOH)
        size = read(data_, DATA_SIZE1);
    else if(code == STX)
        size = read(data_, DATA_SIZE2);

    if(size < ID + CRC16)
        return 0;

    uint16_t crc = (data_[size - 2] << 8) | data_[size - 1];
    if(crc16(data_ + ID, size - ID - CRC16) == crc)
        return size - ID + CRC16;

    return 0;
}

uint64_t XYModemRecvFile::get_filesize(const char* data, uint32_t size)
{
    QString fileSize(data + std::string(data).size() + 1);
    return fileSize.toULongLong();
}

void XYModemRecvFile::doError(QString const& text)
{
    emit error(text);
    serial_->moveToThread(QApplication::instance()->thread());
    emit finished();
}

uint32_t XYModemRecvFile::write(uint8_t const *data, uint32_t size)
{
    return serial_->write((const char *)data, size);
}

uint32_t XYModemRecvFile::read(uint8_t *data, uint32_t size)
{
    return serial_->read((char *)data, size);
}

uint8_t XYModemRecvFile::get_code(bool isWait)
{
    while(!singled())
    {
        if(serial_->waitForReadyRead(10))
        {
            uint8_t data[1] = { 0 };
            read(data, sizeof(data));
            return data[0];
        }
        if(!isWait)
            break;
    }
    return MAX;
}
