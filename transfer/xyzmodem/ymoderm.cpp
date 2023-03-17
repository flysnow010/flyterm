#include "ymoderm.h"
#include <cstdio>
#include <cstring>
#include <iostream>

YModerm::YModerm()
    : XModem()
{
}

bool YModerm::tx_end()
{
    do_end();
    if(get_code() == ACK)
        return true;
    return false;
}

bool YModerm::tx_start(std::string const& filename, std::string const& filesize)
{
    do_start(filename, filesize);
    uint8_t code = get_code();
    if(code != ACK)
        return false;
    code = get_code();
    if(code != C)
        return false;
    return true;
}

void YModerm::do_start(std::string const& filename, std::string const& filesize)
{
    uint8_t frame[FRAME_SIZE1] = { SOH, 0, not_id(0) };
    memcpy(&frame[FRAME_HEAD], filename.c_str(), filename.size());
    memcpy(&frame[FRAME_HEAD + filename.size() + 1], filesize.c_str(), filesize.size());

    uint16_t crc = crc16(frame + FRAME_HEAD, SIZE1);
    frame[FRAME_HEAD + SIZE1] = (uint8_t)(crc >> 8);
    frame[FRAME_HEAD + SIZE1 + 1] = (uint8_t)(crc >> 0);
    write(frame, sizeof(frame));
    next_id();
}

void YModerm::do_end()
{
    uint8_t frame[FRAME_SIZE1] = { SOH, 0, not_id(0) };
    uint16_t crc = crc16(frame + FRAME_HEAD, SIZE1);
    frame[FRAME_HEAD + SIZE1] = (uint8_t)(crc << 8);
    frame[FRAME_HEAD + SIZE1 + 1] = (uint8_t)(crc << 0);
    write(frame, FRAME_SIZE1);
}
