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
    start_send();
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

uint32_t KermitRecvFile::write(char const *data, uint32_t size)
{
    return serial_->write(data, size);
}

uint32_t KermitRecvFile::read(char *data, uint32_t size)
{
    return serial_->read(data, size);
}
