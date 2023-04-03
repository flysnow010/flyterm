#include "kermitrecvfile.h"
#include <QSerialPort>
#include <QFileInfo>
#include <QApplication>

KermitRecvFile::KermitRecvFile(QSerialPort *serial, QObject *parent)
    : QObject(parent)
    , serial_(serial)
    , signal_(false)
{

}

void KermitRecvFile::start(QString const& fileName)
{
    Q_UNUSED(fileName)
    emit finished();
    serial_->moveToThread(QApplication::instance()->thread());
}

void KermitRecvFile::stop()
{
    doSignal();
}

void KermitRecvFile::cancel()
{

}

int KermitRecvFile::write(char const *data, int size)
{
    return serial_->write(data, size);
}

int KermitRecvFile::read(char *data, int size)
{
    return serial_->read(data, size);
}

char KermitRecvFile::getc()
{
    char c = NUL;
    while(!singled())
    {
        if(serial_->waitForReadyRead(10))
        {
            serial_->read(&c, sizeof(c));
            break;
        }
    }
    return c;
}
