#include "xymodemfilesender.h"
#include "xymodemsendfile.h"
#include <QSerialPort>

XYModemFileSender::XYModemFileSender(QSerialPort *serial, bool isYModem, QObject *parent)
    : QObject(parent)
{
    XYModemSendFile* worker = new XYModemSendFile(serial);
    serial->moveToThread(&workerThread);
    worker->moveToThread(&workerThread);

    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    if(isYModem)
        connect(this, &XYModemFileSender::start_send, worker, &XYModemSendFile::startYModem);
    else
        connect(this, &XYModemFileSender::start_send, worker, &XYModemSendFile::startXModem);
    connect(this, &XYModemFileSender::stop_send, worker, &XYModemSendFile::stop);
    connect(this, &XYModemFileSender::cancel_send, worker, &XYModemSendFile::cancel);
    connect(worker, &XYModemSendFile::gotFileSize, this, &XYModemFileSender::gotFileSize);
    connect(worker, &XYModemSendFile::progressInfo, this, &XYModemFileSender::progressInfo);
    connect(worker, &XYModemSendFile::finished, this, &XYModemFileSender::finished);
    connect(worker, &XYModemSendFile::error, this, &XYModemFileSender::error);
    worker_ = worker;
    workerThread.start();
}

XYModemFileSender::~XYModemFileSender()
{
    workerThread.quit();
    workerThread.wait();
}

void XYModemFileSender::start(QString const& fileName)
{
    emit start_send(fileName);
}

void XYModemFileSender::stop()
{
    worker_->stop();
}

void XYModemFileSender::cancel()
{
    worker_->cancel();
}
