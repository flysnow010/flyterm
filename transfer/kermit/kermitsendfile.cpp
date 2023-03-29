#include "kermitsendfile.h"
#include <QSerialPort>
#include <QFileInfo>
#include <QApplication>

KermitSendFile::KermitSendFile(QSerialPort *serial, QObject *parent)
    : QObject(parent)
    , serial_(serial)
    , signal_(false)
{

}

void KermitSendFile::start(QString const& fileName)
{
    Q_UNUSED(fileName)
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

uint32_t KermitSendFile::write(uint8_t const *data, uint32_t size)
{
    return serial_->write((const char *)data, size);
}

uint32_t KermitSendFile::read(uint8_t *data, uint32_t size)
{
    return serial_->read((char *)data, size);
}
