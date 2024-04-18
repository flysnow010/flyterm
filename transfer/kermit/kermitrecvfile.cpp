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
    file_.setFileName(fileName);
    while(!singled() && state_ != SSBRK)
        recv_packet();
    emit finished();
    serial_->moveToThread(QApplication::instance()->thread());
}

void KermitRecvFile::stop()
{
    doSignal();
}

void KermitRecvFile::cancel()
{
    state_ = SSBRK;
}

void KermitRecvFile::on_init(int seq, const char* data, int size)
{
    Kermit::on_init(seq, data, size);
    state_ = SSINT;
}

void KermitRecvFile::on_data(int seq, const char* data, int size)
{
    Q_UNUSED(seq)
    if(state_ == SSINT)
    {
        file_.open(QIODevice::WriteOnly);
        state_ = SSDAT;
    }
    if(file_.isOpen())
    {
        saveData(data, size);
        send_ack(seq);
    }
}

void KermitRecvFile::saveData(const char* data, int size)
{
    char d[MaxLen];
    char a;
    int index = 0;
    int recvSize = 0;

    while(index < size)
    {
        index += decode(data + index, a);
        d[recvSize++] = a;
    }

    file_.write(d, recvSize);

    blockNumber++;
    bytesOfRecv_ += recvSize;
    emit progressInfo(blockNumber, bytesOfRecv_);
}

void KermitRecvFile::on_break(int seq, const char* data, int size)
{
    Q_UNUSED(seq)
    Q_UNUSED(data)
    Q_UNUSED(size)

    state_ = SSBRK;
}

int KermitRecvFile::write(char const *data, int size)
{
    return serial_->write(data, size);
}

int KermitRecvFile::read(char *data, int size)
{
    int read_size = 0;
    while(!singled() && read_size < size)
    {
        if(serial_->waitForReadyRead(timeMs))
            read_size += serial_->read(data + read_size, size - read_size);
    }
    return read_size;
}

char KermitRecvFile::getc()
{
    char c = NUL;
    while(!singled())
    {
        if(serial_->waitForReadyRead(timeMs))
        {
            serial_->getChar(&c);
            break;
        }
    }
    return c;
}
