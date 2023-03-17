#include "consoleparser.h"

ConsoleParser::ConsoleParser()
{
}

void ConsoleParser::parseColor(QString const& c)
{
    if(c.endsWith("30m") || c == "[90m")
        emit onForeColor(ColorRole::Black);
    else if(c == "[40m" || c == "[100m")
        emit onBackColor(ColorRole::Black);
    else if(c.endsWith("31m") || c == "[91m")
        emit onForeColor(ColorRole::Red);
    else if(c == "[41m" || c == "[101m")
        emit onBackColor(ColorRole::Red);
    else if(c.endsWith("32m")  || c == "[92m")
        emit onForeColor(ColorRole::Green);
    else if(c == "[42m" || c == "[102m")
        emit onBackColor(ColorRole::Green);
    else if(c.endsWith("33m")  || c == "[93m")
        emit onForeColor(ColorRole::Yellow);
    else if(c == "[43m" || c == "[103m")
        emit onBackColor(ColorRole::Yellow);
    else if(c.endsWith("34m")  || c == "[94m")
        emit onForeColor(ColorRole::Blue);
    else if(c == "[44m" || c == "[104m")
        emit onBackColor(ColorRole::Blue);
    else if(c.endsWith("35m")  || c == "[95m")
        emit onForeColor(ColorRole::Purple);//Purple
    else if(c == "[45m" || c == "[105m")
        emit onBackColor(ColorRole::Purple);
    else if(c.endsWith("36m")  || c == "[96m")
        emit onForeColor(ColorRole::SkyBlue);//skyblue
    else if(c == "[46m" || c == "[106m")
        emit onBackColor(ColorRole::SkyBlue);
    else if(c.endsWith("37m")  || c == "[97m")
        emit onForeColor(ColorRole::White);
    else if(c == "[47m" || c == "[107m")
        emit onBackColor(ColorRole::White);
    else if(c == "[38;5;9m") //Foreground Extended
        emit onForeColor(ColorRole::Red);
    else if(c == "[38;5;34m")
        emit onForeColor(ColorRole::Green);
    else if(c == "[38;5;27m")
        emit onForeColor(ColorRole::Blue);
    else if(c == "[00m" || c == "[0m" || c == "[m")
        emit onColorClose();
}

void ConsoleParser::parse_n(QString const& n)
{
    if(n == "[6n")//get cursor pos
        emit onGetCursorPos();
}

void ConsoleParser::parse_C(QString const& c)
{
    if(c.isEmpty())
        emit onRight(1);
    else
        emit onRight(c.toInt());
}

void ConsoleParser::parse_D(QString const& d)
{
    if(d.isEmpty())
        emit onLeft(1);
    else
        emit onLeft(d.toInt());
}

