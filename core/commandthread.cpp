#include "commandthread.h"
#include <QMutexLocker>

CommandThread::CommandThread(QObject *parent) : QThread(parent)
{
}

void CommandThread::postCommand(QString const& command)
{
    {
        QMutexLocker locker(&mutex_);
        commands_.push_back(command);
    }
    sem_.release();
}

void CommandThread::stop()
{
    sem_.release();
}

QString CommandThread::command()
{
    QMutexLocker locker(&mutex_);
    if(commands_.isEmpty())
        return QString();
    QString command = commands_.front();
    commands_.pop_front();
    return command;
}

void CommandThread::execCommand(QString const& command)
{
    QStringList commands = command.split(" ");
    if(commands.isEmpty())
        return;

    if(commands[0] == "@sleep")
    {
        if(commands.size() > 1)
            QThread::sleep(commands[1].toULong());
    }
    else if(commands[0] == "@msleep")
    {
        if(commands.size() > 1)
            QThread::msleep(commands[1].toULong());
    }
}

void CommandThread::run()
{
    while(true)
    {
        sem_.acquire();
        QString c = command();
        if(c.isEmpty())
            break;
        emit onAllCommand(c);
        if(c.startsWith("@"))
            execCommand(c);
        else if(c.startsWith("#"))
            emit onExpandCommand(c);
        else if(c.startsWith("!"))
            emit onTestCommand(c);
        else
            emit onCommand(c);
    }
}
