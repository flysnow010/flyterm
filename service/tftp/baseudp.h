#ifndef BASEUDP_H
#define BASEUDP_H
#include <cstdint>

class BaseUdp
{
public:
    BaseUdp();
    virtual ~BaseUdp(){}

    virtual uint32_t write(const char* data, uint32_t size) = 0;
};

#endif // BASEUDP_H
