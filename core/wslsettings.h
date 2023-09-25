#ifndef WSLSETTINGS_H
#define WSLSETTINGS_H
#include <QString>

struct WSLSettings
{
    QString distributionText;
    QString distribution;
    QString startupPath;
    QString specifyUsername;
    bool useSpecifyUsername;

    QString name() const
    {
        if(startupPath.isEmpty())
            return distributionText;
        return QString("%1@%2").arg(distributionText, startupPath);
    }
};
#endif // WSLSETTINGS_H
