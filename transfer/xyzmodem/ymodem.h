#ifndef YMODERM_H
#define YMODERM_H
#include "xmodem.h"
#include <cstdint>
#include <string>

class YModerm : public XModem
{
public:
    YModerm();
protected:
    bool tx_start(std::string const& filename, std::string const& filesize);
    bool tx_end();
private:
    void do_start(std::string const& filename, std::string const& filesize);;
    void do_end();
};

#endif // YMODERM_H
