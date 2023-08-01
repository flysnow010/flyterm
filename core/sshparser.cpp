#include "sshparser.h"
#include <QDebug>

SShParser::SShParser()
{

}

void SShParser::parse(QByteArray const& data)
{
    if(!isEnterKeyPress())
        parseData_.push_back(data);
    else
    {
        setEnterKeyPress(false);
        int index = data.indexOf("\r\n");//ab\r\n or \r\ntext
        if(index == 0)
            parseData_.push_back(data);
        else if(index > 0)
            parseData_.push_back(data.right(data.size() - index - 1));
    }

    const char* start = parseData_.data();
    const char* end = start + parseData_.size();
    const char* ch = start;
    while(ch != end)
    {
        switch(*ch)
        {
            case BEL:
            {
                emit onBeep();
                if(ch > start)
                    emit onText(QString::fromUtf8(start, ch - start));
                start = ch + 1;
                ch++;
                break;
            }
            case CR:
            {
                ch++;
                if(ch != end && *ch == LF)
                {
                    if((ch - 1) > start)
                        emit onText(QString::fromUtf8(start, ch - start - 1));
                    emit onOverWrite(false);
                    start = ch - 1;
                }
                else
                {
                    if((ch - 1) > start)
                        emit onText(QString::fromUtf8(start, ch - start - 1));
                    emit onHome();
                    emit onOverWrite(true);
                    start = ch;
                }

                break;
            }
            case BS:
            {
                if(parseData_.size() == 1 && isLeftKeyPress())
                {
                    setLeftKeyPress(false);
                    emit onLeft(1);
                    parseData_.clear();
                    return;
                }

                if(ch > start && *start != BS)
                    emit onText(QString::fromUtf8(start, ch - start));
                ch += parseBs(ch, end);
                start = ch;
                break;
            }
            case ESC:
            {
                if(ch > start && *start != BS)
                    emit onText(QString::fromUtf8(start, ch - start));
                int size = parseEsc(ch, end);
                if(size == 0)
                {
                    parseData_ = parseData_.right(end - ch);
                    return;
                }
                ch += size;
                start = ch;
                break;
            }
            default:
            {
                if(ch != end)
                    ch++;
                break;
            }
        }

    }

    if(ch > start)
        emit onText(QString::fromUtf8(start, ch - start));
    parseData_.clear();
}

int SShParser::parseBs(const char* start, const char* end)
{
    const char* ch = start + 1;
    while(ch != end)
    {
        if(*ch == BS)
            ch++;
        else
            break;
    }

    int size = ch - start;
    if(!isHomeKeyPress())
        emit onBackspace(size);
    else
    {
        setHomePress(false);
        emit onLeft(size);
    }
    return size;
}

int SShParser::parseEsc(const char* start, const char* end)
{
    const char* ch = start + 1;
    bool isRight = false;
    bool isBrackets = false;
    bool isEnd = false;
    while(ch != end)
    {
        if(*ch == ']')
        {
           isRight = true;
        }
        else if(*ch == ';')
        {
            if(isRight)
            {
                if(QString::fromUtf8(start + 1, ch - start) == "]0;")//BEL end
                {
                    ch += parseTitle(ch, end);
                    isEnd = true;
                    break;
                }
            }
        }
        else if(*ch == 'm')
        {
            parseColor(QString::fromUtf8(start + 1, ch - start));
            ch++;
            isEnd = true;
            break;
        }
        else if(*ch == 'M')//[36M
        {
            parse_M(QString::fromUtf8(start + 2, ch - start - 2));
            ch++;
            isEnd = true;
            break;
        }
        else if(*ch == 'A')
        {
            isEnd = true;
            ch++;
            break;
        }
        else if(*ch == 'B')
        {
            parse_B(QString::fromUtf8(start + 1, ch - start));
            isEnd = true;
            ch++;
            break;
        }
        else if(*ch == 'C')//22C
        {
            parse_C(QString::fromUtf8(start + 2, ch - start - 2));
            isEnd = true;
            ch++;
            break;
        }
        else if(*ch == 'D')//??
        {
            parse_D(QString::fromUtf8(start + 2, ch - start - 2));
            isEnd = true;
            ch++;
            break;
        }
        else if(*ch == 'H')
        {
            parse_H(QString::fromUtf8(start + 2, ch - start - 2));
            isEnd = true;
            ch++;
            break;
        }
        else if(*ch == 'h')
        {
            parse_h(QString::fromUtf8(start + 1, ch - start));
            isEnd = true;
            ch++;
            break;
        }
        else if(*ch == 'G')
        {
            isEnd = true;
            ch++;
            break;
        }
        else if(*ch == 'L')
        {
            parse_L(QString::fromUtf8(start + 2, ch - start - 2));
            isEnd = true;
            ch++;
            break;
        }
        else if(*ch == 'l')
        {
            parse_l(QString::fromUtf8(start + 1, ch - start));
            isEnd = true;
            ch++;
            break;
        }
        else if(*ch == 'J')
        {
            //emit onDelCharToLineEnd();
            parse_J(QString::fromUtf8(start + 1, ch - start));
            ch++;
            isEnd = true;
            break;
        }
        else if(*ch == 'K')
        {
            parse_K(QString::fromUtf8(start + 1, ch - start));
            isEnd = true;
            ch++;
            break;
        }
        else if(*ch == 's')
        {
            isEnd = true;
            ch++;
            break;
        }
        else if(*ch == 'c')//>c
        {
            isEnd = true;
            ch++;
            break;
        }
        else if(*ch == '=')
        {
            isEnd = true;
            ch++;
            break;
        }
        else if(*ch == 't')
        {
            isEnd = true;
            ch++;
            break;
        }
//        else if(*ch == '>') //??
//        {
//            isEnd = true;
//            ch++;
//            break;
//        }
        else if(*ch == 'u')
        {
            isEnd = true;
            ch++;
            break;
        }
        else if(*ch == 'P')
        {
            isEnd = true;
            ch++;
            break;
        }
        else if(*ch == 'r')
        {
            parse_r(QString::fromUtf8(start + 2, ch - start - 2));
            isEnd = true;
            ch++;
            break;
        }
        else if(*ch == 'X')
        {
            isEnd = true;
            ch++;
            break;
        }
        else if(*ch == 'n')//[6n
        {
            parse_n(QString::fromUtf8(start + 1, ch - start));
            isEnd = true;
            ch++;
            break;
        }
        else if(*ch == '(')
        {
            isBrackets = true;
        }
        else if(*ch == '0')
        {
            if(isBrackets)
            {
                parse_0(QString::fromUtf8(start + 1, ch - start));
                isEnd = true;
                ch++;
                break;
            }
        }
        else if(*ch == '@')
        {
            isEnd = true;
            ch++;
            break;
        }
        else if(*ch == ' ')
        {
            isEnd = true;
            ch++;
            break;
        }
        else if(*ch == '?')//]10;? ]11;?
        {
            if(isRight)
            {
                isEnd = true;
                ch++;
                break;
            }
        }
        ch++;
    }
    if(!isEnd)
        return 0;
     else
        return ch - start;
}

int SShParser::parseTitle(const char* start, const char* end)
{
    const char* ch = start + 1;
    while(ch != end)
    {
        if(*ch == BEL)
        {
            emit onTitle(QString::fromUtf8(start + 1, ch - start - 1));
            ch++;
            break;
        }
        ch++;
    }
    return ch - start;
}

void SShParser::parse_0(QString const& c)
{
    if(c == "(0")
        emit onDECLineDrawingMode();
}

void SShParser::parse_B(QString const& b)
{
    if(b == "(B")
        emit onASCIIMode();
}

void SShParser::parse_H(QString const& h)
{
    QStringList tokens = h.split(";");
    if(tokens.size() == 2)
        emit onCursorPos(tokens[0].toInt(), tokens[1].toInt());
}

void SShParser::parse_M(QString const& m)
{
    emit onScrollUp(m.toInt());
}

void SShParser::parse_h(QString const& h)
{
    if(h == "[?1049h")
        emit onSwitchToAlternateScreen();
    else if(h == "[?1h")
        emit onSwitchToAppKeypadMode();
    else if(h == "[?12h")
        emit onCursorStartBlinking();
    else if(h == "[?25h")
        emit onShowCursor();
    else if(h == "[12h")
        emit onSRMOff();
}

void SShParser::parse_L(QString const& l)
{
    if(l.isEmpty())
        emit onScrollDown(1);
    else
        emit onScrollDown(l.toInt());
}

void SShParser::parse_l(QString const& l)
{
    if(l == "[?1049l")
        emit onSwitchToMainScreen();
    else if(l == "[?1l")
        emit onSwitchToNormalKeypadMode();
    else if(l == "[?12l")
        emit onCursorStopBlinking();
    else if(l == "[?25l")
        emit onHideCursor();
    else if(l == "[12l")
        emit onSRMOn();
}

void SShParser::parse_J(QString const& j)
{
    if(j == "[2J")
        emit onCleanScreen();
}

void SShParser::parse_K(QString const& k)
{
    if(k == "[K" || k == "[0K")
        emit onDelCharToLineEnd();
}

void SShParser::parse_r(QString const& r)
{
    QStringList tokens = r.split(";");
    if(tokens.size() == 2)
        emit onRowRangle(tokens[0].toInt(), tokens[1].toInt());
}
