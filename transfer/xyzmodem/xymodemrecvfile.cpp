#include "xymodemrecvfile.h"
#include <QSerialPort>
#include <QDebug>
#include <QApplication>

XYModemRecvFile::XYModemRecvFile(QSerialPort *serial, QObject *parent)
    : QObject(parent)
    , serial_(serial)
    , signal_(false)
{
}

void XYModemRecvFile::startYModem(QString const& fileName)
{
    Q_UNUSED(fileName)
    uint8_t code = wait_start();
    if(code == MAX)
    {
        emit finished();
        serial_->moveToThread(QApplication::instance()->thread());
        return;
    }
    uint32_t size = 0;
    if(code == SOH){
        qDebug() << "read_size:" << DATA_SIZE1;
        size = read(data_, DATA_SIZE1);
    }
    else if(code == STX) {
        qDebug() << "read_size:" << DATA_SIZE2;
        size = read(data_, DATA_SIZE2);
    }
    emit finished();
    serial_->moveToThread(QApplication::instance()->thread());

    qDebug() << "size:" << size;
}

void XYModemRecvFile::startXModem(QString const& fileName)
{
    Q_UNUSED(fileName)
    uint8_t code = wait_start();
    if(code == MAX)
    {
        emit finished();
        serial_->moveToThread(QApplication::instance()->thread());
        return;
    }
    uint32_t size = 0;
    if(code == SOH)
    {
        size = read(data_, DATA_SIZE1);
    }
    else if(code == STX) {
        size = read(data_, DATA_SIZE2);
    }
    emit finished();
    serial_->moveToThread(QApplication::instance()->thread());
    qDebug() << "read_size:" << size;
}

void XYModemRecvFile::stop() { doSignal(); }

void XYModemRecvFile::cancel()
{
    ;
}

uint32_t XYModemRecvFile::write(uint8_t const *data, uint32_t size)
{
    return serial_->write((const char *)data, size);
}

uint32_t XYModemRecvFile::read(uint8_t *data, uint32_t size)
{
    return serial_->read((char *)data, size);
}

uint8_t XYModemRecvFile::get_code()
{
    while(!singled())
    {
        if(serial_->waitForReadyRead(10))
        {
            uint8_t data[1] = { 0 };
            read(data, sizeof(data));
            return data[0];
        }
    }
    return MAX;
}
