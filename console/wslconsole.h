#ifndef WSLCONSOLE_H
#define WSLCONSOLE_H

#include "console.h"

class WSLConsole : public Console
{
    Q_OBJECT
public:
    WSLConsole(QWidget *parent = nullptr);
protected:
     ConsoleParser* createParser() override;
};

#endif // WSLCONSOLE_H
