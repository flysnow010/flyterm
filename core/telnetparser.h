#ifndef TELNETPARSER_H
#define TELNETPARSER_H
#include "consoleparser.h"

class TelnetParser : public ConsoleParser
{
    Q_OBJECT
public:
    TelnetParser();

    //void parse(QByteArray const& data) override;
};

#endif // TELNETPARSER_H
