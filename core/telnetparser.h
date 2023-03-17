#ifndef TELNETPARSER_H
#define TELNETPARSER_H
#include "serialportparser.h"

class TelnetParser : public SerialPortParser
{
    Q_OBJECT
public:
    TelnetParser();
};

#endif // TELNETPARSER_H
