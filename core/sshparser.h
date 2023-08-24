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
     void parse_A(QString const& c);
     void parse_B(QString const& b);
     void parse_d(QString const& d);
     void parse_G(QString const& g);
     void parse_H(QString const& h);
     void parse_f(QString const& f);
     void parse_ICH(QString const& s);
     void parse_P(QString const& f);
     void parse_X(QString const& x);
     void parse_L(QString const& l);
     void parse_M(QString const& m);
     void parse_h(QString const& h);
     void parse_one_h(QString const& h);
     void parse_l(QString const& l);
     void parse_one_l(QString const& l);
     void parse_J(QString const& j);
     void parse_K(QString const& k);
     void parse_Z(QString const& z);
     void parse_r(QString const& r);
     void parse_S(QString const& s);
     void parse_T(QString const& t);
     bool parse_7(QString const& v);
     bool parse_8(QString const& v);
private:
     bool isBracketedPasteMode = false;
};

#endif // SSHPARSER_H
