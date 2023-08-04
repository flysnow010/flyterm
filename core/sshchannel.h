#ifndef SSHCHANNEL_H
#define SSHCHANNEL_H

#include <QObject>

#include "sshsettings.h"
#include "ssh/session.h"
#include "ssh/channel.h"

class SSHChannel : public QObject
{
    Q_OBJECT
public:
    explicit SSHChannel(QObject *parent = nullptr);

    int write(QByteArray const& data);
public slots:
    void connectTo(SSHSettings const& settings);
    bool run();
    void shellSize(int cols, int rows);
    void stop();
signals:
    void connected();
    void unconnected();
    void onData(QByteArray const& data);
    void onError(QByteArray const& data);
    void connectionError(QString const& error);
private:
    bool singled() { return signal_; }
    void doSignal() { signal_ = true; };
private:
    ssh::Session::Ptr sessioin_;
    ssh::Channel* channel_;
    volatile bool signal_;
    volatile bool shellSizeChanged_;
    bool isConnected = false;
    int cols_;
    int rows_;
};

#endif // SSHCHANNEL_H
