#ifndef LOCALSHELLCONSOLE_H
#define LOCALSHELLCONSOLE_H

#include "console.h"

class LocalShellConsole : public Console
{
    Q_OBJECT
public:
    LocalShellConsole(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *e) override;
private:
    QString command;
};

#endif // LOCALSHELLCONSOLE_H
