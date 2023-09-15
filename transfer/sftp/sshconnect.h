#ifndef SSHCONNECT_H
#define SSHCONNECT_H
#include "core/sshsettings.h"
#include "ssh/session.h"

#include <QObject>

class SshConnect : public QObject
{
    Q_OBJECT
public:
    explicit SshConnect(QObject *parent = nullptr);

    std::string userName() const { return username_; }
    ssh::Session::Ptr sessioin() const { return sessioin_; }
public slots:
    void start(SSHSettings const& settings);
    void stop();

signals:
    void connected();
    void unconnected();
    void connectionError(QString const& error);
private:
    ssh::Session::Ptr sessioin_;
    std::string username_;
};

#endif // SSHCONNECT_H
