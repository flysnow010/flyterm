#include "wslconsole.h"
#include "core/sshparser.h"

WSLConsole::WSLConsole(QWidget *parent)
    : Console(parent)
{
}

ConsoleParser* WSLConsole::createParser()
{
    return new SShParser();
}
