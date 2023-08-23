#ifndef SERIALPORTCONSOLE_H
#define SERIALPORTCONSOLE_H

#include "console.h"

class SerialPortConsole : public Console
{
    Q_OBJECT
public:
    explicit SerialPortConsole(QWidget *parent = nullptr);

protected:
    ConsoleParser * createParser() override;
};

#endif // SERIALPORTCONSOLE_H
