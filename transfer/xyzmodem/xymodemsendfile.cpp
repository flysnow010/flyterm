#include "xymodemsendfile.h"
#include <QSerialPort>
#include <iostream>
#include <QFileInfo>
#include <QApplication>

XYModemSendFile::XYModemSendFile(QSerialPort *serial, QObject *parent)
    : QObject(parent)
    , serial_(serial)
    , signal_(false)
{
}

void XYModemSendFile::startYModem(QString const& fileName)
{
    QFileInfo fileInfo(fileName);
    uint64_t fileSize = fileInfo.size();
    std::string filename = fileInfo.fileName().toStdString();
    std::string filesize = QString::number(fileSize).toStdString();
    if(filename.size() + filesize.size() + 2 > SIZE1)
    {
        doError("Filename too long!");
        return;
    }
    //Get Start flag
    if(get_code() != C)
    {
        doError("Can not get  C!");
        return;
    }
    //SOH first block

    emit gotFileSize(fileSize);
    if(!tx_start(filename, filesize))
    {
        doError("Can not get  ACK and C!");
        return;
    }
    //Send data of file
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
    {
        doError(QString("%1 cannot be opened!").arg(fileName));
        return;
    }

    uint8_t data[SIZE2];
    quint64 bytesOfSend = 0;
    quint32 block = 0;
    bool is_end = false;

    while(!singled())
    {
        quint64 size = file.read((char *)data, sizeof(data));
        if(size == 0)
        {
            is_end = true;
            break;
        }
        if(tx_send(data, size))
        {
            block++;
            bytesOfSend += size;
            emit progressInfo(block, bytesOfSend);
        }
        else
        {
            emit error("send is error");
            break;
        }
    }

    //Send end
    if(is_end)
    {
        if(tx_eot())
        {
            if(tx_end())
            {
                //std::cout << "end is ok" << std::endl;
            }
        }
    }
    emit finished();
    serial_->moveToThread(QApplication::instance()->thread());
}

void XYModemSendFile::startXModem(QString const& fileName)
{
    QFileInfo fileInfo(fileName);
    uint64_t fileSize = fileInfo.size();
    //Get Start flag
    if(get_code() != C)
    {
        doError("Can not get  C!");
        return;
    }
    //SOH first block
    emit gotFileSize(fileSize);
    //Send data of file
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
    {
        doError(QString("%1 cannot be opened!").arg(fileName));
        return;
    }

    uint8_t data[SIZE2];
    quint64 bytesOfSend = 0;
    quint32 block = 0;
    bool is_end = false;
    next_id();
    while(!singled())
    {
        quint64 size = file.read((char *)data, sizeof(data));
        if(size == 0)
        {
            is_end = true;
            break;
        }
        if(tx_send(data, size))
        {
            block++;
            bytesOfSend += size;
            emit progressInfo(block, bytesOfSend);
        }
        else
        {
            emit error("send is error");
            break;
        }
    }

    //Send end
    if(is_end)
    {
        if(tx_eot())
        {
            //std::cout << "end is ok" << std::endl;
        }
    }
    emit finished();
    serial_->moveToThread(QApplication::instance()->thread());
}

void XYModemSendFile::stop() { doSignal(); }

void XYModemSendFile::cancel() { tx_cancel(); }

void XYModemSendFile::doError(QString const& text)
{
    emit error(text);
    serial_->moveToThread(QApplication::instance()->thread());
    emit finished();
}

uint32_t XYModemSendFile::write(uint8_t const *data, uint32_t size)
{
    return serial_->write((const char *)data, size);
}

uint32_t XYModemSendFile::read(uint8_t *data, uint32_t size)
{
    return serial_->read((char *)data, size);
}

uint8_t XYModemSendFile::get_code(bool isWait)
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
