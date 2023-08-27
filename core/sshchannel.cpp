#include "sshchannel.h"
#include <QThread>
#include <QDebug>

int const DEFAULT_COLS = 80;
int const DEFAULT_ROWS = 24;

SSHChannel::SSHChannel(QObject *parent)
    : QObject(parent)
    , sessioin_(new ssh::Session())
    , channel_(0)
    , signal_(false)
    , shellSizeChanged_(false)
    , cols_(DEFAULT_COLS)
    , rows_(DEFAULT_ROWS)
{
}

int SSHChannel::write(QByteArray const& data)
{
    if(!isConnected)
        return 0;

    return channel_->write((void *)data.data(), data.size());
}

bool SSHChannel::read(QByteArray &data)
{
    QMutexLocker locker(&mutex);
    if(datas.isEmpty())
        return false;
    data = datas.takeFirst();
    while(datas.size() > 0 && data.size() < 4096)
        data.push_back(datas.takeFirst());
    qDebug() << "getData: " << data.size() << "/" << datas.size();
    return true;
}

void SSHChannel::shellSize(int cols, int rows)
{
    if((cols == cols_ && rows == rows_)
            || (cols < 0 || rows < 0))
        return;

    cols_ = cols;
    rows_ =  rows;
    shellSizeChanged_ = true;
}

void SSHChannel::connectTo(SSHSettings const& settings)
{
    sessioin_->set_host(settings.hostName.toStdString().c_str());
    sessioin_->set_port(settings.port);

    if(!sessioin_->connect())
    {
        emit connectionError(sessioin_->error());
        return;
    }

    if(!sessioin_->verify())
    {
        emit connectionError("verify is failed");
        return;
    }

    if(settings.usePrivateKey)
    {
        if(!sessioin_->login_by_prikey(settings.userName.toStdString().c_str(),
                             settings.privateKeyFileName.toStdString().c_str()))
        {
            emit connectionError("login is failed");
            return;
        }
    }
    else
    {
        if(!sessioin_->login(settings.userName.toStdString().c_str(),
                             settings.passWord.toStdString().c_str()))
        {
            emit connectionError("login is failed");
            return;
        }
    }
    channel_ = new ssh::Channel(*sessioin_);
    channel_->open();
    channel_->run_shell(cols_, rows_);

    emit connected();
    isConnected = true;
}

bool SSHChannel::run()
{
    while(!singled())
    {
        int nbytes = channel_->poll();
        if(shellSizeChanged_)
        {
            channel_->shell_size(cols_, rows_);
            shellSizeChanged_ = false;
        }
        if(nbytes <= 0)
        {
            QThread::msleep(1);
            continue;
        }

        QByteArray bytes(nbytes, Qt::Uninitialized);
        channel_->read(bytes.data(), bytes.size());
        addData(bytes);
    }
    return true;
}

void SSHChannel::addData(QByteArray const& data)
{
    QMutexLocker locker(&mutex);
    datas.push_back(data);
}

void SSHChannel::stop()
{
    doSignal();
    sessioin_->disconnect();
    emit unconnected();
}
