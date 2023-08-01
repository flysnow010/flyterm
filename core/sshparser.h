#ifndef SSHPARSER_H
#define SSHPARSER_H
#include "consoleparser.h"

class SShParser : public ConsoleParser
{
    Q_OBJECT
public:
    SShParser();
    void parse(QByteArray const& data) override;
private:
     int parseBs(const char* start, const char* end);
     int parseEsc(const char* start, const char* end);
     int parseTitle(const char* start, const char* end);
     void parse_0(QString const& c);
     void parse_B(QString const& b);
     void parse_H(QString const& h);
     void parse_M(QString const& m);
     void parse_h(QString const& h);
     void parse_L(QString const& l);
     void parse_l(QString const& l);
     void parse_J(QString const& j);
     void parse_K(QString const& k);
     void parse_r(QString const& r);
};

#endif // SSHPARSER_H
