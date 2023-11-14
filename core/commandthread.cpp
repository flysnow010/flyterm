#include "commandthread.h"
#include "util/util.h"
#include <QMutexLocker>
#include <QProcess>

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

void CommandThread::runOrderCommand()
{
    sem_.release();
}

void CommandThread::stop()
{
    sem_.release();
    doSignal();
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

QString CommandThread::orderCommand()
{
    if(orderCommands_.isEmpty())
        return QString();
    QString command = orderCommands_.front();
    orderCommands_.pop_front();
    return command;
}

void CommandThread::pushOrderCommand(QString const& orderCommand)
{
    QString command = orderCommand.right(orderCommand.size() - 1);
    if(!command.startsWith("start"))
        orderCommands_.push_back(command);
    else
    {
        QStringList cmds = command.split(' ');
        if(cmds.size() < 2)
            orderCommands_.clear();
        else
            emit onOrderCommandStart(cmds[1]);
    }
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

void CommandThread::execAppCommand(QString const& command)
{
    QStringList commands = command.split(" ");
    if(commands.isEmpty())
        return;

    QProcess process;
    if(commands[0] == "$plot")
        process.setProgram(Util::plotAppPath());
    else
        process.setProgram(commands[0].mid(1));
    process.setArguments(commands.mid(1));
    process.startDetached();
}

void CommandThread::execNextOrderCommand()
{
    if(!orderCommands_.isEmpty())
        runOrderCommand();
}

void CommandThread::run()
{
    while(!singled())
    {
        sem_.acquire();
        QString c = command();
        if(c.isEmpty())
        {
            c = orderCommand();
            if(c.isEmpty())
                continue;

            if(c.startsWith("@"))
            {
                execCommand(c);
                execNextOrderCommand();
            }
            else if(c.startsWith("$"))
            {
                execAppCommand(c);
                execNextOrderCommand();
            }
            else if(c.startsWith("#"))
            {
                emit onExpandCommand(c);
                execNextOrderCommand();
            }
            else
                emit onCommand(c);
            if(orderCommands_.isEmpty())
                emit onOrderCommandEnd();
        }
        else
        {
            if(c.startsWith("!"))
            {
                pushOrderCommand(c);
                continue;
            }
            else if(c.startsWith("@"))
                execCommand(c);
            else if(c.startsWith("$"))
                execAppCommand(c);
            else if(c.startsWith("#"))
                emit onExpandCommand(c);
            else
                emit onCommand(c);
        }
        emit onAllCommand(c);
    }
}
