#include "consoleparser.h"

ConsoleParser::ConsoleParser()
{
}

void ConsoleParser::parseOneSGR(QString const& sgr)
{
    if(sgr == "" || sgr == "0" || sgr == "00")
        emit onColorClose();
    else if(sgr == "1")
        emit onBold();
    else if(sgr == "4")
        emit onUnderLine();
    else if(sgr == "5")
        emit onBlink();
    else if(sgr == "7")
        emit onReverse();
    else if(sgr == "22'")
        emit onNoBold();
    else if(sgr == "24'")
        emit onNoUnderLine();
    else if(sgr == "27'")
        emit onNormalColor();
    else if(sgr == "39")
    {
        emit onForeColor(ColorRole::NullRole);
        emit onNormalForeColor();//??
    }
    else if(sgr == "49")
    {
        emit onBackColor(ColorRole::NullRole);
        emit onNormalBackColor();//??
    }
    else if(sgr == "30" || sgr == "90")
        emit onForeColor(ColorRole::Black);
    else if(sgr == "31" || sgr == "91")
        emit onForeColor(ColorRole::Red);
    else if(sgr == "32" || sgr == "92")
        emit onForeColor(ColorRole::Green);
    else if(sgr == "33" || sgr == "93")
        emit onForeColor(ColorRole::Yellow);
    else if(sgr == "34" || sgr == "94")
        emit onForeColor(ColorRole::Blue);
    else if(sgr == "35" || sgr == "95")
        emit onForeColor(ColorRole::Purple);
    else if(sgr == "36" || sgr == "96")
        emit onForeColor(ColorRole::SkyBlue);
    else if(sgr == "37" || sgr == "97")
        emit onForeColor(ColorRole::White);
    else if(sgr == "40" || sgr == "100")
        emit onBackColor(ColorRole::Black);
    else if(sgr == "41" || sgr == "101")
        emit onBackColor(ColorRole::Red);
    else if(sgr == "42" || sgr == "102")
        emit onBackColor(ColorRole::Green);
    else if(sgr == "43" || sgr == "103")
        emit onBackColor(ColorRole::Yellow);
    else if(sgr == "44" || sgr == "104")
        emit onBackColor(ColorRole::Blue);
    else if(sgr == "45" || sgr == "105")
        emit onBackColor(ColorRole::Purple);
    else if(sgr == "46" || sgr == "106")
        emit onBackColor(ColorRole::SkyBlue);
    else if(sgr == "47" || sgr == "107")
        emit onBackColor(ColorRole::White);
}

void ConsoleParser::parseSGR(QString const& sgr)
{
    QStringList tokens = sgr.split(";");
    if(tokens.isEmpty())
        return;
    if(tokens[0] == "38" && tokens.size() >= 3)//forecolor
    {
        if(tokens[1] == "5")//index
        {
            if(tokens[2] == "9")
                emit onForeColor(ColorRole::Red);
            else if(tokens[2] == "34")
                emit onForeColor(ColorRole::Green);
            else if(tokens[2] == "27")
                emit onForeColor(ColorRole::Blue);
            else if(tokens[2] == "62")
                emit onForeColor(ColorRole::SkyBlue);
        }
        else if(tokens[1] == "2")//rgb
        {

        }
    }
    else if(tokens[0] == "48" && tokens.size() >= 3)
    {
        if(tokens[1] == "5")//index
        {
            if(tokens[2] == "250")
                emit onBackColor(ColorRole::White);
        }
        else if(tokens[1] == "2")//rgb
        {

        }
    }
    else
    {
        foreach(auto token, tokens)
            parseOneSGR(token);
    }

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

