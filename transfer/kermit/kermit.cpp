#include "kermit.h"

Kermit::Kermit()
{
}

void Kermit::start_send()
{
    int length = 3;
    data_[++length] = tochar(maxl);
    data_[++length] = tochar(time);
    data_[++length] = tochar(npad);
    data_[++length] = tochar(padc);
    data_[++length] = tochar(eol);
    data_[++length] = qctl;

    data_[0] = MARK;
    data_[1] = tochar(length);
    data_[2] = tochar(0);
    data_[3] = S;
    data_[++length] = '\0';
    data_[length] = tochar(check(&data_[1]));
    data_[++length] = his_eol;
    send_packe(data_, length + 1);
}

void Kermit::ack(int n)
{
    data_[0] = MARK;
    data_[1] = tochar(3);
    data_[2] = tochar(n);
    data_[3] = Y;
    data_[4] = '\0';
    data_[4] = tochar(check(&data_[1]));
    data_[5] = his_eol;
    send_packe(data_, 6);
}

void Kermit::nack(int n)
{
    data_[0] = MARK;
    data_[1] = tochar(3);
    data_[2] = tochar(n);
    data_[3] = N;
    data_[4] = '\0';
    data_[4] = tochar(check(&data_[1]));
    data_[5] = his_eol;
    send_packe(data_, 6);
}

void Kermit::send_packe(const char* data, uint32_t size)
{
    write(data, size);
}

uint16_t Kermit::check(const char* p)
{
    uint16_t s = checksum(p);
    uint16_t t = (s + ((s >> 6) & 0x03)) & 0x3F;
    return t;
}

uint16_t Kermit::checksum(const char* p)
{
    uint16_t s = 0;
    while(*p)
         s += *p++;
    return s;
}
