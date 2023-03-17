#ifndef TELNETSETTINGS_H
#define TELNETSETTINGS_H

#include <QString>

struct TelnetSettings
{
    QString hostName;
    QString userName;
    int port = 23;

    QString name() const { return QString("%1 (%2)").arg(hostName).arg(userName); }
};
#endif // TELNETSETTINGS_H
