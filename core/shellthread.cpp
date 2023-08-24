#include "shellthread.h"
#include <QMutexLocker>

ShellThread::ShellThread(QObject *parent) : QThread(parent)
{
}

ShellThread::~ShellThread()
{
    doSignal();
    quit();
    wait();
}

void ShellThread::addData(QByteArray const& data)
{
    QMutexLocker locker(&mutex);
    byteArrayList.push_back(data);
}

void ShellThread::run()
{
    while(!singled())
    {
        QByteArray data;
        {
            QMutexLocker locker(&mutex);
            if(!byteArrayList.isEmpty())
            {
                data = byteArrayList.join();
                byteArrayList.clear();
            }
        }
        if(!data.isEmpty())
            emit onData(data);
        else
            QThread::msleep(10);
    }
}
