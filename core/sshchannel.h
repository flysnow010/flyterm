#ifndef SSHCHANNEL_H
#define SSHCHANNEL_H

#include <QObject>
#include <QByteArrayList>
#include <QMutex>

#include "sshsettings.h"
#include "ssh/session.h"
#include "ssh/channel.h"

class SSHChannel : public QObject
{
    Q_OBJECT
public:
    explicit SSHChannel(QObject *parent = nullptr);
    ~SSHChannel();

    int write(QByteArray const& data);
    bool read(QByteArray &data);
    void reset();
public slots:
    void connectTo(SSHSettings const& settings);
    bool run();
    void shellSize(int cols, int rows);
    void stop();
signals:
    void connected();
    void unconnected();
    void onError(QByteArray const& data);
    void connectionError(QString const& error);
private:
    bool singled() { return signal_; }
    void doSignal() { signal_ = true; };

    void addData(QByteArray const& data);
private:
    ssh::Session::Ptr sessioin_;
    ssh::Channel* channel_;
    QByteArrayList datas;
    QMutex mutex;
    volatile bool signal_;
    volatile bool shellSizeChanged_;
    bool isConnected = false;
    int cols_;
    int rows_;
};

#endif // SSHCHANNEL_H
