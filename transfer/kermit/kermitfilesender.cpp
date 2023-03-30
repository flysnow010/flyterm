#include "kermitfilesender.h"
#include "kermitsendfile.h"
#include <QSerialPort>

KermitFileSender::KermitFileSender(QSerialPort *serial, QObject *parent)
    : QObject(parent)
{
    KermitSendFile* worker = new KermitSendFile(serial);

    serial->moveToThread(&workerThread);
    worker->moveToThread(&workerThread);

    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &KermitFileSender::start_send, worker, &KermitSendFile::start);
    connect(this, &KermitFileSender::stop_send, worker, &KermitSendFile::stop);
    connect(this, &KermitFileSender::cancel_send, worker, &KermitSendFile::cancel);
    connect(worker, &KermitSendFile::gotFileSize, this, &KermitFileSender::gotFileSize);
    connect(worker, &KermitSendFile::progressInfo, this, &KermitFileSender::progressInfo);
    connect(worker, &KermitSendFile::finished, this, &KermitFileSender::finished);
    connect(worker, &KermitSendFile::error, this, &KermitFileSender::error);

    workerThread.start();
}

KermitFileSender::~KermitFileSender()
{
    workerThread.quit();
    workerThread.wait();
}

void KermitFileSender::start(QString const& fileName)
{
    emit start_send(fileName);
}

void KermitFileSender::stop()
{
    emit stop_send();
}

void KermitFileSender::cancel()
{
    emit cancel_send();
}
