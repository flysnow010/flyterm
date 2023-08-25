#include "shellthread.h"
#include <QMutexLocker>
#include "core/consoleparser.h"
#include <QDebug>

ShellThread::ShellThread(QObject *parent) : QThread(parent)
{
    qRegisterMetaType<ColorRole>("ColorRole");
}

ShellThread::~ShellThread()
{
    doSignal();
    quit();
    wait();
}

bool ShellThread::getData(QByteArray &data)
{
    QMutexLocker locker(&mutex);
    if(byteArrayList.isEmpty())
        return false;
    data = byteArrayList.takeFirst();
    while(byteArrayList.size() > 0 && data.size() < 8192)
        data.push_back(byteArrayList.takeFirst());
    qDebug() << "getData: " << data.size() << "/" << byteArrayList.size();
    return true;
}

void ShellThread::addData(QByteArray const& data)
{
    QMutexLocker locker(&mutex);
    byteArrayList.push_back(data);
}

void ShellThread::setCommandParser(ConsoleParser* parser)
{
    commandParser = parser;
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
        {
            if(commandParser)
                commandParser->parse(data);
            else
                emit onData(data);
        }
        else
            QThread::msleep(10);
    }
}
