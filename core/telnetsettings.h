#ifndef TELNETSETTINGS_H
#define TELNETSETTINGS_H

#include <QString>

struct TelnetSettings
{
    QString hostName;
    QString userName;
    int port = 23;

    QString name() const { return QString("%1@%)").arg(userName, hostName); }
};
#endif // TELNETSETTINGS_H
