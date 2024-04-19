#include "xymodemfilerecver.h"
#include "xymodemrecvfile.h"
#include <QSerialPort>

XYModemFileRecver::XYModemFileRecver(QSerialPort *serial, bool isYModem, QObject *parent)
    : QObject(parent)
{
    worker = new XYModemRecvFile(serial);
    serial->moveToThread(&workerThread);
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    if(isYModem)
        connect(this, &XYModemFileRecver::start_recv, worker, &XYModemRecvFile::startYModem);
    else
        connect(this, &XYModemFileRecver::start_recv, worker, &XYModemRecvFile::startXModem);

    connect(this, &XYModemFileRecver::stop_recv, worker, &XYModemRecvFile::stop);
    connect(this, &XYModemFileRecver::cancel_recv, worker, &XYModemRecvFile::cancel);

    connect(worker, &XYModemRecvFile::gotFileSize, this, &XYModemFileRecver::gotFileSize);
    connect(worker, &XYModemRecvFile::progressInfo, this, &XYModemFileRecver::progressInfo);
    connect(worker, &XYModemRecvFile::finished, this, &XYModemFileRecver::finished);
    connect(worker, &XYModemRecvFile::error, this, &XYModemFileRecver::error);

    workerThread.start();
}

XYModemFileRecver::~XYModemFileRecver()
{
    workerThread.quit();
    workerThread.wait();
}

void XYModemFileRecver::start(QString const& fileName)
{
    emit start_recv(fileName);
}

void XYModemFileRecver::stop()
{
    worker->stop();
}

void XYModemFileRecver::cancel()
{
    worker->cancel();
}
