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

void LocalShell::setWorkingDirectory(QString const& directory)
{
    process->setWorkingDirectory(directory);
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
    QByteArray data = process->readAllStandardOutput();
    emit onData(data);
}

void LocalShell::readyReadStderr()
{
    QByteArray data = process->readAllStandardError();
    emit onData(data);
}

void LocalShell::wait()
{
    process->waitForFinished();
}

void LocalShell::stop()
{
   process->kill();
   process->waitForFinished(1000);
}
