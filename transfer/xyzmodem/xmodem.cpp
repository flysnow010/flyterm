#include "xmodem.h"
#include <cstdio>
#include <cstring>
#include <iostream>

XModem::XModem()
    : id_(0x0)
{

}

bool XModem::tx_send(uint8_t *data, uint32_t size, int max_count)
{
    if(size > SIZE2)
        return false;

    do_send(data, size);
    for(int i = 0; i < max_count; i++)
    {
        uint8_t code = get_code();
        if(code == MAX)
            break;
        else if(code == NAK)
            do_send(data, size);
        else if(code == ACK)
        {
            next_id();
            return true;
        }
    }
    return false;
}

bool XModem::tx_eot()
{
    do_eot();
    uint8_t code = get_code();
    if(code == NAK)
    {
        std::cout << "need next file" << std::endl;
        do_eot();
        if(get_code() == ACK)
            return true;
    }
    else if(code == ACK)
    {
        return true;
    }
    return false;
}

uint8_t XModem::wait_start(int max_count)
{
    for(int i = 0; i < max_count; i++)
    {
        do_c();
        uint8_t code = get_code();
        if(code != MAX)
            return code;
    }
    return MAX;
}

void XModem::do_send(uint8_t const* data, uint16_t size)
{
    uint16_t data_size = SIZE2;

    memset(frame_, 0, sizeof(frame_));

    frame_[1] = id_;
    frame_[2] =  not_id(id_);

    if(size > SIZE1)
        frame_[0] = STX;
    else
    {
        frame_[0] = SOH;
        data_size = SIZE1;
    }

    memcpy(&frame_[FRAME_HEAD], data, size);
    memset(&frame_[FRAME_HEAD + size], 0x1A, data_size - size);

    uint16_t crc = crc16(&frame_[FRAME_HEAD], data_size);
    frame_[FRAME_HEAD + data_size] = (uint8_t)(crc >> 8);
    frame_[FRAME_HEAD + data_size + 1] = (uint8_t)(crc >> 0);

    write(frame_, (data_size == SIZE1 ? FRAME_SIZE1 : FRAME_SIZE2));
}

void XModem::tx_cancel()
{
    uint8_t frame[] = { CAN, CAN, CAN, CAN, CAN };
    write(frame, sizeof(frame));
}


void XModem::tx_code(Code code)
{
    uint8_t frame = code;
    write(&frame, CODE);
}

