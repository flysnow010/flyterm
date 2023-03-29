#include "kermit.h"

Kermit::Kermit()
{
}

uint16_t Kermit::check(const char* p)
{
    uint16_t s = checksum(p);
    uint16_t t = (((s & 192) >> 6) + s) & 63;
    return t;
}

uint16_t Kermit::checksum(const char* p)
{
    uint16_t s;
    uint16_t m = 0377;

    for(s = 0; *p != '\0'; p++)
        s += *p & m;
    return s;
}
