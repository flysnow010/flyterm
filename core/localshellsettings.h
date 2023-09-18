#ifndef LOCALSHELLSETTINGS_H
#define LOCALSHELLSETTINGS_H
#include <QString>

struct LocalShellSettings
{
    QString shellText;
    QString shellType;
    QString currentPath;

    QString getCurrentPath() const
    {
        QString windowsPath = currentPath;
        windowsPath.replace("/", "\\");
        return windowsPath;
    }
    QString name() const
    {
        if(currentPath.isEmpty())
            return shellText;
        return QString("%1@%2").arg(shellText, currentPath);
    }
};
#endif // LOCALSHELLSETTINGS_H
