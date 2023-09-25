#ifndef LOCALSHELLSETTINGS_H
#define LOCALSHELLSETTINGS_H
#include <QString>

struct LocalShellSettings
{
    QString shellText;
    QString shellType;
    QString startupPath;
    QString executeCommand;

    QString getCurrentPath() const
    {
        QString windowsPath = startupPath;
        windowsPath.replace("/", "\\");
        return windowsPath;
    }
    QString name() const
    {
        if(startupPath.isEmpty())
            return shellText;
        return QString("%1@%2").arg(shellText, startupPath);
    }
};
#endif // LOCALSHELLSETTINGS_H
