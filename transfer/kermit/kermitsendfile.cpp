#include "kermitsendfile.h"
#include <QSerialPort>
#include <QFileInfo>
#include <QApplication>
#include <QDebug>

KermitSendFile::KermitSendFile(QSerialPort *serial, QObject *parent)
    : QObject(parent)
    , serial_(serial)
    , signal_(false)
{
}

void KermitSendFile::start(QString const& fileName)
{
    QFileInfo fileInfo(fileName);
    emit gotFileSize(fileInfo.size());

    send_init();
    state_ = SSINT;
    file_.setFileName(fileName);
    while(!singled() && state_ != SSBRK)
    {
        recv_packet();
    }
    emit finished();
    serial_->moveToThread(QApplication::instance()->thread());
}

void KermitSendFile::on_ack(int seq, const char* data, int size)
{
    if(seq == 0 && state_ == SSINT)
    {
        state_ = SSDAT;
        file_.open(QIODevice::ReadOnly);
        Kermit::on_ack(seq, data, size);
    }
    qDebug() << "ack: " << seq;
    if(state_ == SSEND)
    {
        state_ = SSBRK;
        return;
    }

    if(!file_.atEnd())
        send_data(next_seq(seq));
    else
    {
        send_break(next_seq(seq));
        state_ = SSEND;
    }
}

void KermitSendFile::send_data(int seq)
{
    char data[MaxLen];//94 93
    int size = 0;
    char c;
    while(!singled() && size < MaxLen - 1)
    {
        if(getchar(c))
            size += encode(c, data + size);
        else
            break;
    }
    if(!singled() && size > 0)
    {
        qDebug() << "send: " << seq;
        Kermit::send_data(seq, data, size);

        blockNumber++;
        bytesOfSend_ += size;
        emit progressInfo(blockNumber, bytesOfSend_);
    }
}

void KermitSendFile::on_nack(int seq, const char* data, int size)
{
    if(state_ == SSDAT)
        resend();
    Kermit::on_nack(seq, data, size);
}

void KermitSendFile::on_error(int seq, const char* data, int size)
{
    Kermit::on_error(seq, data, size);
}

void KermitSendFile::stop()
{
    doSignal();
}

void KermitSendFile::cancel()
{
    state_ = SSBRK;//??
}

bool KermitSendFile::getchar(char &c)
{
    if(file_.read(&c, sizeof(c)) == 1)
        return true;
    return false;
}

char KermitSendFile::getc()
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

int KermitSendFile::write(char const *data, int size)
{
    return serial_->write(data, size);
}

int KermitSendFile::read(char *data, int size)
{
    int read_size = 0;
    while(!singled() && read_size < size) {
        if(serial_->waitForReadyRead(10))
            read_size += serial_->read(data + read_size, size - read_size);
    }
    return read_size;
}
