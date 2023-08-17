#ifndef SSHSETTINGS_H
#define SSHSETTINGS_H
#include <QString>

struct SSHSettings
{
    QString hostName;
    QString userName;
    QString passWord;
    QString privateKeyFileName;
    int port = 22;
    bool usePrivateKey = false;

    QString name() const { return QString("%1@%2").arg(userName, hostName); }
    QString key() const { return QString("%1@%2:%3").arg(userName, hostName).arg(port); }
};

#endif // SSHSETTINGS_H
