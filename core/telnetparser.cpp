#include "telnetparser.h"
#include <QDebug>

TelnetParser::TelnetParser()
{
}

void TelnetParser::parse(QByteArray const& data)
{
    if(isRightKeyPress())
    {
        setRightKeyPress(false);
        emit onRight(1);
        return;
    }

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
            case BS:
            {
                if(parseData_.size() == 1)
                {
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
