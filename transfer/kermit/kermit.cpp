#include "kermit.h"
#include <vector>
#include <iostream>
#include <string>

Kermit::Kermit()
{
}

void Kermit::on_init(int seq, const char* data, uint32_t size)
{
    std::cout << "on_init(" << seq << "," << std::string(data, size) << std::endl;
}

void Kermit::on_file_header(int seq, const char* data, uint32_t size)
{
    std::cout << "on_file_header(" << seq << "," << std::string(data, size) << std::endl;
}

void Kermit::on_data(int seq, const char* data, uint32_t size)
{
    std::cout << "on_data(" << seq << "," << std::string(data, size) << std::endl;
}

void Kermit::on_end(int seq, const char* data, uint32_t size)
{
    std::cout << "on_end(" << seq << "," << std::string(data, size) << std::endl;
}

void Kermit::on_ack(int seq, const char* data, uint32_t size)
{
    std::cout << "on_ack(" << seq << "," << std::string(data, size) << std::endl;
    if(size > 0)
        maxl  = unchar(data[0]);
    if(size > 1)
        time  = unchar(data[1]);
    if(size > 2)
        npad  = unchar(data[2]);
    if(size > 3)
        padc  = unchar(data[3]);
    if(size > 4)
        eol  = unchar(data[4]);
    if(size > 5)
        qctl  = data[5];
    std::cout << "maxl: " << maxl << std::endl;
    std::cout << "time: " << time << std::endl;
    std::cout << "npad: " << npad << std::endl;
    std::cout << "padc: " << padc << std::endl;
    std::cout << "eol: " << (int)eol << std::endl;
    std::cout << "qctl: " << qctl << std::endl;
}

void Kermit::on_nack(int seq, const char* data, uint32_t size)
{
    std::cout << "on_nack(" << seq << "," << std::string(data, size) << std::endl;
}

void Kermit::on_error(int seq, const char* data, uint32_t size)
{
    std::cout << "on_error(" << seq << "," << std::string(data, size) << std::endl;
}

void Kermit::send_init()
{
    uint32_t length = 3;
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
    data_[++length] = eol;
    send_packet(data_, length + 1);
}

void Kermit::send_data(const char* data, uint32_t size)
{

}

void Kermit::send_ack(int n)
{
    uint32_t i = 0;
    data_[i++] = MARK;
    data_[i++] = tochar(3);
    data_[i++] = tochar(n);
    data_[i++] = Y;
    data_[i] = '\0';
    data_[i++] = tochar(check(&data_[1]));
    data_[i++] = eol;
    send_packet(data_, i);
}

void Kermit::send_nack(int n)
{
    uint32_t i = 0;
    data_[i++] = MARK;
    data_[i++] = tochar(3);
    data_[i++] = tochar(n);
    data_[i++] = N;
    data_[i] = '\0';
    data_[i++] = tochar(check(&data_[1]));
    data_[i++] = eol;
    send_packet(data_, i);
}

bool  Kermit::send_packet(const char* data, uint32_t size)
{
    return write(data, size) == size;
}

bool Kermit::recv_packet()
{
    char ch = getch();
    if(ch != MARK)
        return false;

    ch = getch();
    int length = unchar(ch);
    if(length < MinSize)
        return false;

    //SEQ  TYPE DATA CHECK <terminator>
    std::vector<char> data(length + 1, 0);
    if(read(data.data(), data.size()) != data.size())
        return false;

    if(data.back() != eol)
        return false;

    uint16_t  old_check = unchar(data.at(data.size() - 2));
    uint16_t  new_check = check(ch, data.data(), data.data() + data.size() - 2);
    if(old_check != new_check)
        return false;

    char type = data[1];
    if(type == S)
        on_init(unchar(data[0]), data.data() + 2, data.size() - 4);
    else if(type == F)
        on_file_header(unchar(data[0]), data.data() + 2, data.size() - 4);
    else if(type == D)
        on_data(unchar(data[0]), data.data() + 2, data.size() - 4);
    else if(type == Z)
        on_end(unchar(data[0]), data.data() + 2, data.size() - 4);
    else if(type == Y)
        on_ack(unchar(data[0]), data.data() + 2, data.size() - 4);
    else if(type == N)
        on_nack(unchar(data[0]), data.data() + 2, data.size() - 4);
    else if(type == E)
        on_error(unchar(data[0]), data.data() + 2, data.size() - 4);
    return true;
}

char Kermit::ktrans(char in)
{
    if ((in & 0x60) == 0x40)
        return (char) (in & ~0x40);
    else if ((in & 0x7f) == 0x3f)
        return (char) (in | 0x40);
    else
        return in;
}

uint16_t Kermit::check(const char* p)
{
    uint16_t s = 0;
    while(*p)
         s += *p++;

    return (s + ((s >> 6) & 0x03)) & 0x3F;
}

uint16_t Kermit::check(uint16_t s, const char* begin, const char* end)
{
    while(begin != end)
        s += *begin++;
    return (s + ((s >> 6) & 0x03)) & 0x3F;
}
