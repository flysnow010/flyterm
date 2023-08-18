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
                    emit onText(toText(start, ch - start));
                start = ch + 1;
                ch++;
                break;
            }
            case SO:
            {
                if(ch > start)
                    emit onText(toText(start, ch - start));
                emit onDECLineDrawingMode();
                start = ch + 1;
                ch++;
                break;
            }
            case SI:
            {
                if(ch > start)
                    emit onText(toText(start, ch - start));
                emit onASCIIMode();
                start = ch + 1;
                ch++;
                break;
            }
//            case CR:
//            {
//                ch++;
//                if(ch != end && *ch == LF)
//                {
//                    if((ch - 1) > start)
//                        emit onText(toText(start, ch - start - 1));
//                    emit onOverWrite(false);
//                    start = ch - 1;
//                }
//                else
//                {
//                    if((ch - 1) > start)
//                        emit onText(toText(start, ch - start - 1));
//                    emit onHome();
//                    emit onOverWrite(true);
//                    start = ch;
//                }

//                break;
//            }
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
                    emit onText(toText(start, ch - start));
                ch += parseBs(ch, end);
                start = ch;
                break;
            }
            case ESC:
            {
                if(ch > start && *start != BS)
                    emit onText(toText(start, ch - start));
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
        emit onText(toText(start, ch - start));
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
    if(isHomeKeyPress())
    {
        setHomePress(false);
        emit onLeft(size);
    }
    else if(isLeftKeyPress())
    {
        setLeftKeyPressCount(-size);
        emit onLeft(size);
    }
    else
        emit onBackspace(size);
    return size;
}

int SShParser::parseEsc(const char* start, const char* end)
{
    const char* ch = start + 1;
    bool isRight = false;
    bool isLeft = false;
    bool isBrackets = false;
    bool isEnd = false;
    while(ch != end)
    {
        if(*ch == ']')
        {
           isRight = true;
        }
        else if(*ch == '[')
        {
            isLeft = true;
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
            if(isLeft)
                parseSGR(QString::fromUtf8(start + 2, ch - start - 2));
            else
                parseSGR(QString::fromUtf8(start + 1, ch - start -1));
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
            parse_A(QString::fromUtf8(start + 2, ch - start - 2));
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
            parse_h(QString::fromUtf8(start + 2, ch - start - 2));
            isEnd = true;
            ch++;
            break;
        }
        else if(*ch == 'f')
        {
            parse_f(QString::fromUtf8(start + 2, ch - start - 2));
            isEnd = true;
            ch++;
            break;
        }
        else if(*ch == 'G')
        {
            parse_G(QString::fromUtf8(start + 2, ch - start - 2));
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
            parse_l(QString::fromUtf8(start + 2, ch - start - 2));
            isEnd = true;
            ch++;
            break;
        }
        else if(*ch == 'J')
        {
            //emit onDelCharToLineEnd();
            parse_J(QString::fromUtf8(start + 2, ch - start - 2));
            ch++;
            isEnd = true;
            break;
        }
        else if(*ch == 'K')
        {
            parse_K(QString::fromUtf8(start + 2, ch - start - 2));
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
        else if(*ch == 'S')
        {
            parse_S(QString::fromUtf8(start + 2, ch - start - 2));
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
        else if(*ch == 'd')
        {
            parse_d(QString::fromUtf8(start + 2, ch - start - 2));
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
        else if(*ch == 'T')
        {
            parse_T(QString::fromUtf8(start + 2, ch - start - 2));
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
        else if(*ch == 'P')//[2P
        {
            parse_P(QString::fromUtf8(start + 2, ch - start - 2));
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
            parse_X(QString::fromUtf8(start + 2, ch - start - 2));
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
        else if(*ch == '(' || *ch == ')')
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
        else if(*ch == 'Z')
        {
            parse_Z(QString::fromUtf8(start + 2, ch - start - 2));
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
        else if(*ch == '7')
        {
            isEnd = parse_7(QString::fromUtf8(start + 1, ch - start));
            if(isEnd)
            {
                ch++;
                break;
            }
        }
        else if(*ch == '8')
        {
            isEnd = parse_8(QString::fromUtf8(start + 1, ch - start));
            if(isEnd)
            {
                ch++;
                break;
            }
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
    else if(c == ")0")
        ;
}

void SShParser::parse_A(QString const& a)
{
    if(a.isEmpty())
        emit onUp(1);
    else
        emit onUp(a.toInt());
}

void SShParser::parse_B(QString const& b)
{
    if(b == "(B")
        emit onASCIIMode();
}

void SShParser::parse_d(QString const& d)
{
    if(d.isEmpty())
        emit onRow(1);
    else
        emit onRow(d.toInt());
}

void SShParser::parse_G(QString const& g)
{
    if(g.isEmpty())
        emit onCol(1);
    else
        emit onCol(g.toInt());
}

void SShParser::parse_H(QString const& h)
{
    QStringList tokens = h.split(";");
    if(tokens.size() == 2)
        emit onCursorPos(tokens[0].toInt(), tokens[1].toInt());
    else
        emit onScreenHome();
}

void SShParser::parse_f(QString const& f)
{
    QStringList tokens = f.split(";");
    if(tokens.size() == 2)
        emit onCursorPos(tokens[0].toInt(), tokens[1].toInt());
}

void SShParser::parse_P(QString const& p)
{
    if(p.isEmpty())
        emit onDelChars(1);
    else
        emit onDelChars(p.toInt());
}

void SShParser::parse_M(QString const& m)
{
    if(m.isEmpty())
        emit onDeleteLine(1);
    else
        emit onDeleteLine(m.toInt());
}

void SShParser::parse_h(QString const& h)//4h
{
    if(h.isEmpty())
        return;

    if(!h.startsWith('?'))
    {
        if(h == "2")
            emit onKAMOn();
        else if(h == "4")
            emit onIRMOn();
        else if(h == "12")
            emit onSRMOn();
        else if(h == "20")
            emit onLNMOn();
    }
    else
    {
        QStringList tokens = h.mid(1).split(";");
        foreach(auto token, tokens)
            parse_one_h(token);
    }
}

void SShParser::parse_one_h(QString const& h)
{
    if(h == "1049")
    {
        if(isBracketedPasteMode)
            emit onSwitchToAlternateCharScreen();
        else
            emit onSwitchToAlternateVideoScreen();
    }
    else if(h == "2004")
        isBracketedPasteMode = true;
    else if(h == "47")
        emit onSwitchToAlternateVideoScreen();
    else if(h == "1")
        emit onSwitchToAppKeypadMode();
    else if(h == "12")
        emit onCursorStartBlinking();
    else if(h == "25")
        emit onShowCursor();
}

void SShParser::parse_L(QString const& l)
{
    if(l.isEmpty())
        emit onInsertLine(1);
    else
        emit onInsertLine(l.toInt());
}

void SShParser::parse_l(QString const& l)//4l
{
    if(!l.startsWith('?'))
    {
        if(l == "2")
            emit onKAMOff();
        else if(l == "4")
            emit onIRMOff();
        else if(l == "12")
            emit onSRMOff();
        else if(l == "20")
            emit onLNMOff();
    }
    else
    {
        QStringList tokens = l.mid(1).split(";");
        foreach(auto token, tokens)
            parse_one_l(token);
    }
}

void SShParser::parse_one_l(QString const& l)
{
    if(l == "1049" || l == "47")
        emit onSwitchToMainScreen();
    if(l == "2004")
        isBracketedPasteMode = false;
    else if(l == "1")
        emit onSwitchToNormalKeypadMode();
    else if(l == "12")
        emit onCursorStopBlinking();
    else if(l == "25")
        emit onHideCursor();
}

void SShParser::parse_J(QString const& j)
{
    if(j.isEmpty() || j == "0")
        emit onCleanToScreenEnd();
    else if(j == "1")
        emit onCleanToScreenHome();
    else if(j == "2")
        emit onCleanScreen();
}

void SShParser::parse_K(QString const& k)
{
    if(k.isEmpty() || k == "0")
        emit onDelCharToLineEnd();
    else if(k == "1")
        emit onDelCharToLineHome();
    else if(k == "2")
        emit onDelCharOfLine();
}

void SShParser::parse_X(QString const& x)
{
    if(x.isEmpty())
        emit onEraseChars(1);
    else
        emit onEraseChars(x.toInt());
}

void SShParser::parse_Z(QString const& z)
{
    if(z.isEmpty())
        emit onLeft(4);//tab = 4 spaces
    else
        emit onLeft(z.toInt() * 4);
}

void SShParser::parse_r(QString const& r)
{
    QStringList tokens = r.split(";");
    if(tokens.size() == 2)
        emit onRowRangle(tokens[0].toInt(), tokens[1].toInt());
}

void SShParser::parse_S(QString const& s)
{
    if(s.isEmpty())
        emit onScrollUp(1);
    else
        emit onScrollUp(s.toInt());
}

void SShParser::parse_T(QString const& t)
{
    if(t.isEmpty())
        emit onScrollDown(1);
    else
        emit onScrollDown(t.toInt());
}

bool SShParser::parse_7(QString const& v)
{
    if(v == "7")
    {
        emit onSaveCursorPos();
        return true;
    }
    return false;
}

bool SShParser::parse_8(QString const& v)
{
    if(v == "8")
    {
        emit onRestoreCursorPos();
        return true;
    }
    return false;
}
