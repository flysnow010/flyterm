#ifndef CONSOLECHAR_H
#define CONSOLECHAR_H
#include <QChar>
#include <QString>
#include <QByteArray>
#include <color/consolepalette.h>

struct ConsoleChar
{
    char value = 0;
    bool isDrawLineMode_ = false;
    ConsolRole role;
    inline void reset()
    {
      value = 0;
      isDrawLineMode_ = false;
    }
};

struct ConsoleText
{
    QByteArray text;
    ConsolRole role;
};

#endif // CONSOLECHAR_H
