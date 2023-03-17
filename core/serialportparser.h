#ifndef SERIALPORTPARSER_H
#define SERIALPORTPARSER_H
#include "consoleparser.h"

class SerialPortParser : public ConsoleParser
{
    Q_OBJECT
public:
    SerialPortParser();

    void parse(QByteArray const& data) override;
private:
     int parseBs(const char* start, const char* end);
     int parseEsc(const char* start, const char* end);
};

#endif // SERIALPORTPARSER_H
