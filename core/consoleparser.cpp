#include "consoleparser.h"
#include <QTextCodec>

ConsoleParser::ConsoleParser()
    : textCodec(QTextCodec::codecForName("UTF-8"))
{
}

void ConsoleParser::setCodecName(QByteArray const& name)
{
    if(textCodec->name() != name)
    {
        QTextCodec* codec = QTextCodec::codecForName(name);
        if(codec)
           textCodec = codec;
    }
}

QString ConsoleParser::toText(const char *str, int size)
{
    return textCodec->toUnicode(str, size);
}

void ConsoleParser::parseOneSGR(QString const& sgr)
{
    if(sgr == "" || sgr == "0" || sgr == "00")
        emit onColorClose();
    else if(sgr == "1")
        emit onBold(true);
    else if(sgr == "4")
        emit onUnderLine(true);
    else if(sgr == "5")
        emit onBlink();
    else if(sgr == "7")
        emit onReverse();
    else if(sgr == "22'")
        emit onBold(false);
    else if(sgr == "24'")
        emit onUnderLine(false);
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
    else if(sgr == "30")
        emit onForeColor(ColorRole::Black);
    else if(sgr == "90")
        emit onForeColor(ColorRole::BrightBlack);
    else if(sgr == "31")
        emit onForeColor(ColorRole::Red);
    else if(sgr == "91")
        emit onForeColor(ColorRole::BrightRed);
    else if(sgr == "32")
        emit onForeColor(ColorRole::Green);
    else if(sgr == "92")
        emit onForeColor(ColorRole::BrightGreen);
    else if(sgr == "33")
        emit onForeColor(ColorRole::Yellow);
    else if(sgr == "93")
        emit onForeColor(ColorRole::BrightYellow);
    else if(sgr == "34")
        emit onForeColor(ColorRole::Blue);
    else if(sgr == "94")
        emit onForeColor(ColorRole::BrightBlue);
    else if(sgr == "35")
        emit onForeColor(ColorRole::Purple);
    else if(sgr == "95")
        emit onForeColor(ColorRole::BrightPurple);
    else if(sgr == "36")
        emit onForeColor(ColorRole::Cyan);
    else if(sgr == "96")
        emit onForeColor(ColorRole::BrightCyan);
    else if(sgr == "37")
        emit onForeColor(ColorRole::White);
    else if(sgr == "97")
        emit onForeColor(ColorRole::BrightWhite);
    else if(sgr == "40")
        emit onBackColor(ColorRole::Black);
    else if(sgr == "100")
        emit onBackColor(ColorRole::BrightBlack);
    else if(sgr == "41")
        emit onBackColor(ColorRole::Red);
    else if(sgr == "101")
        emit onBackColor(ColorRole::BrightRed);
    else if(sgr == "42")
        emit onBackColor(ColorRole::Green);
    else if(sgr == "102")
        emit onBackColor(ColorRole::BrightGreen);
    else if(sgr == "43")
        emit onBackColor(ColorRole::Yellow);
    else if(sgr == "103")
        emit onBackColor(ColorRole::BrightYellow);
    else if(sgr == "44")
        emit onBackColor(ColorRole::Blue);
    else if(sgr == "104")
        emit onBackColor(ColorRole::BrightBlue);
    else if(sgr == "45")
        emit onBackColor(ColorRole::Purple);
    else if(sgr == "105")
        emit onBackColor(ColorRole::BrightPurple);
    else if(sgr == "46")
        emit onBackColor(ColorRole::Cyan);
    else if(sgr == "106")
        emit onBackColor(ColorRole::BrightCyan);
    else if(sgr == "47")
        emit onBackColor(ColorRole::White);
    else if(sgr == "107")
        emit onBackColor(ColorRole::BrightWhite);
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
            emit onForeColor(toColorRole(tokens[2].toInt()));
        }
        else if(tokens[1] == "2")//rgb
        {
        }
    }
    else if(tokens[0] == "48" && tokens.size() >= 3)
    {
        if(tokens[1] == "5")//index
        {
            emit onBackColor(toColorRole(tokens[2].toInt()));
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

