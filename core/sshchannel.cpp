#include "sshchannel.h"
#include <QThread>
#include <QDebug>

int const DEFAULT_COLS = 80;
int const DEFAULT_ROWS = 24;
unsigned long const SLEEP_TIME_1_MS = 1;
unsigned long const WAIT_TIME_5_MS  = 5;

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

SSHChannel::~SSHChannel()
{
    delete channel_;
    channel_ = 0;
}

int SSHChannel::write(QByteArray const& data)
{
    if(!isConnected)
        return 0;
    return channel_->write((void *)data.data(), data.size());
}

void SSHChannel::reset()
{
    sessioin_ = ssh::Session::Ptr(new ssh::Session());
    delete channel_;
    channel_ = 0;
    signal_ = false;
    shellSizeChanged_ = false;
}

bool SSHChannel::read(QByteArray &data)
{
    QMutexLocker locker(&mutex);
    if(datas.isEmpty())
        return false;
    data = datas.takeFirst();
    while(datas.size() > 0 && data.size() < 4096)
        data.push_back(datas.takeFirst());
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

    ssh::AuthMethod method = sessioin_->auth_method(
                settings.userName.toStdString().c_str());

    if(ssh::isUnknowOfAuthMethod(method))
    {
        emit connectionError("Unknow auth method!");
        return;
    }
    else if(ssh::isPubKeyOfAuthMethod(method) && settings.usePrivateKey)
    {
        if(!sessioin_->login_by_prikey(
                    settings.userName.toStdString().c_str(),
                    settings.privateKeyFileName.toStdString().c_str()))
        {
            emit connectionError("Pubkey's login is failed!");
            return;
        }
    }
    else if(ssh::isPasswordOfAuthMethod(method))
    {
        if(!sessioin_->login(settings.userName.toStdString().c_str(),
                             settings.passWord.toStdString().c_str()))
        {
            emit connectionError("Password's login is failed!");
            return;
        }
    }
    else if(ssh::isInteractiveOfAuthMethod(method))
    {
        if(!sessioin_->login_by_interactive(
                    settings.userName.toStdString().c_str(),
                    settings.passWord.toStdString().c_str()))
        {
            emit connectionError("Interactive's login is failed!");
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
            QThread::msleep(SLEEP_TIME_1_MS);
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
    if(channel_)
        channel_->close();
    doSignal();
    //sleep 5ms for wait exit of run
    QThread::msleep(WAIT_TIME_5_MS);
    sessioin_->disconnect();
    if(channel_)
        channel_->reset();
    isConnected = false;
    emit unconnected();
}
