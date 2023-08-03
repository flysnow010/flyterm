#ifndef CONSOLECHAR_H
#define CONSOLECHAR_H
#include <QChar>
#include <QString>
#include <QByteArray>
#include <color/consolepalette.h>

struct ConsoleChar
{
    char value = 0;
    bool isDrawLineMode = false;
    ConsolRole role;
    inline void reset()
    {
      value = 0;
      isDrawLineMode = false;
    }
};

struct ConsoleText
{
    QByteArray text;
    ConsolRole role;
};

#endif // CONSOLECHAR_H
