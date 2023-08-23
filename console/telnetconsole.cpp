#include "telnetconsole.h"
#include "core/telnetparser.h"

TelnetConsole::TelnetConsole(QWidget *parent)
    : Console(parent)
{
}

ConsoleParser * TelnetConsole::createParser()
{
    return new TelnetParser();
}

