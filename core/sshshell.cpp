#include "sshshell.h"
#include "sshchannel.h"

SshShell::SshShell(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<SSHSettings>("SSHSettings");
    SSHChannel* worker = new SSHChannel();

    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &SshShell::connectReq, worker, &SSHChannel::connectTo);
    connect(this, &SshShell::runReq, worker, &SSHChannel::run);
    connect(worker, &SSHChannel::connected, this, &SshShell::connected);
    connect(worker, &SSHChannel::unconnected, this, &SshShell::unconnected);
    connect(worker, &SSHChannel::connectionError, this, &SshShell::connectionError);
    connect(worker, &SSHChannel::onData, this, &SshShell::onData);
    connect(worker, &SSHChannel::onError, this, &SshShell::onError);


    worker_ = worker;
    workerThread.start();
}

SshShell::~SshShell()
{
    workerThread.quit();
    workerThread.wait();
}

void SshShell::connectTo(SSHSettings const& settings)
{
    emit connectReq(settings);
}

void SshShell::run(int cols, int rows)
{
    emit runReq(cols, rows);
}

int SshShell::write(QByteArray const& data)
{
    return worker_->write(data);
}

void SshShell::stop()
{
    worker_->stop();
}

