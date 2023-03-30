#include "kermitsendfile.h"
#include <QSerialPort>
#include <QFileInfo>
#include <QApplication>
#include <QDebug>
#include <QByteArray>

KermitSendFile::KermitSendFile(QSerialPort *serial, QObject *parent)
    : QObject(parent)
    , serial_(serial)
    , signal_(false)
{

}

void KermitSendFile::start(QString const& fileName)
{
    Q_UNUSED(fileName)
    qDebug() << "readBufferSize:" << serial_->readBufferSize();
    start_send();
    getAck();
    emit finished();
    serial_->moveToThread(QApplication::instance()->thread());
}

void KermitSendFile::stop()
{
    doSignal();
}

void KermitSendFile::cancel()
{

}

bool KermitSendFile::getAck()
{
    char c;

    if(!getc(&c))
        return false;
    if(c != MARK)
        return false;
    if(!getc(&c))
        return false;
    int length = unchar(c);
    int i = 0;
    QByteArray data(length + 2, 0);
    while(!singled() && i < length) {
        if(getc(&c))
        {
           data[i] = c;
           i++;
        }
    }
    qDebug() << data;
    return true;
}

int KermitSendFile::getc(char *c)
{
    while(!singled())
    {
        if(serial_->waitForReadyRead(10))
            return read(c, 1);
    }
    return 0;
}

uint32_t KermitSendFile::write(char const *data, uint32_t size)
{
    return serial_->write(data, size);
}

uint32_t KermitSendFile::read(char *data, uint32_t size)
{
    return serial_->read(data, size);
}
