#include "serialportparser.h"

SerialPortParser::SerialPortParser()
{
}

void SerialPortParser::parse(QByteArray const& data)
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

int SerialPortParser::parseBs(const char* start, const char* end)
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
    emit onBackspace(size);
    return size;
}

int SerialPortParser::parseEsc(const char* start, const char* end)
{
    const char* ch = start + 1;
    while(ch != end)
    {
        if(*ch == 'm')
        {
            parseColor(QString::fromUtf8(start + 1, ch - start));
            return ch - start + 1;
        }
        else if(*ch == 'n')//[6n
        {
            parse_n(QString::fromUtf8(start + 1, ch - start));
            return ch - start + 1;
        }
        else if(*ch == 'C')//[999C
        {
            parse_C(QString::fromUtf8(start + 2, ch - start - 2));
            return ch - start + 1;
        }
        else if(*ch == 'D')//[1D
        {
            parse_D(QString::fromUtf8(start + 2, ch - start - 2));
            return ch - start + 1;
        }
        else if(*ch == 'J')
        {
            emit onDelCharToLineEnd();
            return ch - start + 1;
        }
        ch++;
    }

    return 0;
}
