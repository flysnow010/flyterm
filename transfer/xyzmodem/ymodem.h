#ifndef YMODEM_H
#define YMODEM_H
#include "xmodem.h"
#include <cstdint>
#include <string>

class YModem : public XModem
{
public:
    YModem();
protected:
    bool tx_start(std::string const& filename, std::string const& filesize);
    bool tx_end();
private:
    void do_start(std::string const& filename, std::string const& filesize);;
    void do_end();
};

#endif // YMODEM_H
