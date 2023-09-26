#ifndef TELNETCONSOLE_H
#define TELNETCONSOLE_H

#include "console.h"

class TelnetConsole : public Console
{
    Q_OBJECT
public:
    explicit TelnetConsole(QWidget *parent = nullptr);

protected:
    ConsoleParser* createParser() override;
};

#endif // TELNETCONSOLE_H
