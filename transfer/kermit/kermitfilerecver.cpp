#include "kermitfilerecver.h"
#include "kermitrecvfile.h"
#include <QSerialPort>

KermitFileRecver::KermitFileRecver(QSerialPort *serial, QObject *parent)
    : QObject(parent)
{
    KermitRecvFile* worker = new KermitRecvFile(serial);
    serial->moveToThread(&workerThread);
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &KermitFileRecver::start_recv, worker, &KermitRecvFile::start);
    connect(this, &KermitFileRecver::stop_recv, worker, &KermitRecvFile::stop);
    connect(this, &KermitFileRecver::cancel_recv, worker, &KermitRecvFile::cancel);

    connect(worker, &KermitRecvFile::gotFileSize, this, &KermitFileRecver::gotFileSize);
    connect(worker, &KermitRecvFile::progressInfo, this, &KermitFileRecver::progressInfo);
    connect(worker, &KermitRecvFile::finished, this, &KermitFileRecver::finished);
    connect(worker, &KermitRecvFile::error, this, &KermitFileRecver::error);

    workerThread.start();
}

KermitFileRecver::~KermitFileRecver()
{
    workerThread.quit();
    workerThread.wait();
}

void KermitFileRecver::start(QString const& fileName)
{
    emit start_recv(fileName);
}

void KermitFileRecver::stop()
{
    emit stop_recv();
}

void KermitFileRecver::cancel()
{
    emit cancel_recv();
}
