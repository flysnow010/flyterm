#ifndef XMODEM_H
#define XMODEM_H
#include "modem.h"
#include <cstdint>
#include <string>

class XModem : public Modem
{
public:
    XModem();

    enum Code {
        NUL = 0x00,
        SOH = 0x01,
        STX = 0x02,
        EOT = 0x04,
        ACK = 0x06,
        NAK = 0x15,
        CAN = 0x18,
        C   = 0x43,
        MAX = 0xff
    };
    enum Size {
        CODE = 1,
        ID = 2,
        SIZE1 = 128,
        SIZE2 = 1024,
        CRC16 = 2,
        FRAME_HEAD  = CODE + ID, //1 + 2
        FRAME_SIZE1 = FRAME_HEAD + SIZE1 + CRC16,// 3 + 128 + 2 = 133
        FRAME_SIZE2 = FRAME_HEAD + SIZE2 + CRC16, // 3 + 1024 + 2 = 1029
        DATA_SIZE1 = FRAME_SIZE1 -1,
        DATA_SIZE2 = FRAME_SIZE2 -1
    };
protected:
    virtual uint8_t get_code() = 0;

    bool tx_send(uint8_t *data, uint32_t size, int max_count = 5);
    bool tx_eot();
    bool tx_end();
    void tx_cancel();

    uint8_t not_id(uint8_t id) { return 0xFF - id; }
    void next_id();
private:
    uint8_t next_id(uint8_t id) { return (id + 1) % 0x100; }
    void do_send(uint8_t const* data, uint16_t size);
    void do_eot();
private:
    uint8_t id_;
    uint8_t frame_[FRAME_SIZE2];
};

#endif // XMODEM_H
