#ifndef CONSOLECHAR_H
#define CONSOLECHAR_H
#include <QChar>
#include <QString>
#include <color/consolepalette.h>

struct ConsoleChar
{
    QChar value;
    ConsolRole role;
};

struct ConsoleText
{
    QString text;
    ConsolRole role;
};

#endif // CONSOLECHAR_H
