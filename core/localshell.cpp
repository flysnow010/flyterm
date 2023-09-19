#include "localshell.h"
#include <QProcess>
#include <QDebug>

LocalShell::LocalShell(QObject *parent)
    : QObject(parent)
    , process(new QProcess(this))
{
    connect(process, &QProcess::readyReadStandardOutput,
            this, &LocalShell::readyReadStdout);
    connect(process, &QProcess::readyReadStandardError,
            this, &LocalShell::readyReadStderr);
    process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());
}

void LocalShell::write(QByteArray const& data)
{
    process->write(data);
}

void LocalShell::start(QString const& shell)
{
    process->setProgram(shell);
    process->start();
}

void LocalShell::start(QString const& shell, QStringList const& params)
{
    process->setProgram(shell);
    process->setArguments(params);
    process->start();
}

void LocalShell::readyReadStdout()
{
    process->setCurrentReadChannel(0);
    QByteArray data = process->readAll();
    emit onData(data);
}

void LocalShell::readyReadStderr()
{
    process->setCurrentReadChannel(1);
    QByteArray data = process->readAll();
    emit onData(data);
}

void LocalShell::stop()
{
   process->kill();
   process->waitForFinished(1000);
}
