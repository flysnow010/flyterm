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
    send_init();
    recv_packet();
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

char KermitSendFile::getch()
{
    char c = NUL;
    while(!singled()) {
        if(serial_->waitForReadyRead(10)) {
            serial_->read(&c, sizeof(c));
            break;
        }
    }
    return c;
}

uint32_t KermitSendFile::write(char const *data, uint32_t size)
{
    return serial_->write(data, size);
}

uint32_t KermitSendFile::read(char *data, uint32_t size)
{
    uint32_t read_size = 0;
    while(!singled() && read_size < size) {
        if(serial_->waitForReadyRead(10))
            read_size += serial_->read(data + read_size, size - read_size);
    }
    return read_size;
}
