#include "sshchannel.h"

SSHChannel::SSHChannel(QObject *parent)
    : QObject(parent)
    , sessioin_(new ssh::Session())
    , channel_(0)
    , signal_(false)
{

}

int SSHChannel::write(QByteArray const& data)
{
    return channel_->write((void *)data.data(), data.size());
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
    emit connected();
}

bool SSHChannel::run(int cols, int rows)
{
    if(!channel_->open())
        return false;

   if(!channel_->run_shell(cols, rows))
       return false;

    char buffer[256];
    while(!singled())
    {
        int nbytes = channel_->read(buffer, sizeof(buffer));
        if(nbytes == 0)
            continue;

        emit onData(QByteArray(buffer, nbytes));
    }
    return true;
}

void SSHChannel::stop()
{
    doSignal();
    sessioin_->disconnect();
    emit unconnected();
}
