#ifndef MODEM_H
#define MODEM_H
#include <cstdint>

class Modem
{
public:
    Modem();
protected:
    virtual uint32_t write(uint8_t const *data, uint32_t size) = 0;
    virtual uint32_t read(uint8_t *data, uint32_t size) = 0;
    uint16_t crc16(uint8_t const *data, uint32_t size);
};

#endif // MODEM_H
