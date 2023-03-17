#include "modem.h"

Modem::Modem()
{

}

uint16_t Modem::crc16(uint8_t const *data, uint32_t size)
{
    uint16_t crc = 0;
    while(size--)
    {
        crc ^= (uint16_t)(*(data++)) << 8;

        for(int i = 0; i < 8; i++)
        {
            if(crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc = crc << 1;
        }
    }
    return crc;
}
