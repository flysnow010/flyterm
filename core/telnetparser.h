#ifndef TELNETPARSER_H
#define TELNETPARSER_H
#include "serialportparser.h"

class TelnetParser : public SerialPortParser
{
    Q_OBJECT
public:
    TelnetParser();

    void parse(QByteArray const& data) override;
};

#endif // TELNETPARSER_H
