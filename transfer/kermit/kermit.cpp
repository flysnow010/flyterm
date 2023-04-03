#include "kermit.h"
#include <vector>
#include <iostream>
#include <string>

Kermit::Kermit()
{
}

void Kermit::on_init(int seq, const char* data, int size)
{
    std::cout << "on_init(" << seq << "," << std::string(data, size) << std::endl;
}

void Kermit::on_file_header(int seq, const char* data, int size)
{
    std::cout << "on_file_header(" << seq << "," << std::string(data, size) << std::endl;
}

void Kermit::on_data(int seq, const char* data, int size)
{
    std::cout << "on_data(" << seq << "," << std::string(data, size) << std::endl;
}

void Kermit::on_end(int seq, const char* data, int size)
{
    std::cout << "on_end(" << seq << "," << std::string(data, size) << std::endl;
}

void Kermit::on_ack(int seq, const char* data, int size)
{
    //std::cout << "on_ack(" << seq << "," << std::string(data, size) << std::endl;
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
//    std::cout << "maxl: " << maxl << std::endl;
//    std::cout << "time: " << time << std::endl;
//    std::cout << "npad: " << npad << std::endl;
//    std::cout << "padc: " << padc << std::endl;
//    std::cout << "eol: " << (int)eol << std::endl;
//    std::cout << "qctl: " << qctl << std::endl;
}

void Kermit::on_nack(int seq, const char* data, int size)
{
    std::cout << "on_nack(" << seq << "," << std::string(data, size) << std::endl;
}

void Kermit::on_error(int seq, const char* data, int size)
{
    std::cout << "on_error(" << seq << "," << std::string(data, size) << std::endl;
}

void Kermit::send_init()
{
    char data[6];
    data[0] = tochar(maxl);
    data[1] = tochar(time);
    data[2] = tochar(npad);
    data[3] = tochar(padc);
    data[4] = tochar(eol);
    data[5] = qctl;
    spack(S, 0, data, sizeof (data));
}

void Kermit::send_data(int n, const char* data, int len)
{
    spack(D, n, data, len);
}

void Kermit::send_end(int n)
{
    spack(Z, n, nullptr, 0);
}

void Kermit::send_break(int n)
{
    spack(B, n, nullptr, 0);
}

int Kermit::encode(char a, char* data)
{
    int a7 = a & 127;
    int size = 0;
    if (a7 < 32 || a7 == 127)
    {
        data[size++] = qctl;
        a = ctl(a);
    }
    else if (a7 == qctl)
    {
        data[size++] = qctl;
    }
    data[size++] = a;
    data[size] = '\0';
    return size;
}

int Kermit::decode(const char* data, char &b)
{
    const char *d = data;
    int a = *d++;
    if(a == qctl) {
        a = *d++;
        int a7 = a & 127;
        if(a7 < 62 && a7 < 96)
            a = ctl(a);
    }
    b = a;
    return d - data;
}

int decode(char* data, char a);

void Kermit::send_ack(int n)
{
    spack(Y, n, nullptr, 0);
}

void Kermit::send_nack(int n)
{
    spack(N, n, nullptr, 0);
}

bool  Kermit::send_packet(const char* data, int size)
{
    last_size = size;
    return write(data, size) == size;
}

bool Kermit::recv_packet()
{
    char ch = getc();
    if(ch != MARK)
        return false;

    ch = getc();
    int length = unchar(ch);
    if(length < MinLen)
        return false;

    //SEQ  TYPE DATA CHECK <terminator>
    std::vector<char> data(length + 1, 0);
    if(read(data.data(), data.size()) != static_cast<int>(data.size()))
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

void Kermit::resend()
{
    send_packet(data_, last_size);
}

int Kermit::check(const char* p)
{
    int s = 0;
    while(*p)
         s += *p++;

    return (s + ((s >> 6) & 0x03)) & 0x3F;
}

int Kermit::check(int s, const char* begin, const char* end)
{
    while(begin != end)
        s += *begin++;
    return (s + ((s >> 6) & 0x03)) & 0x3F;
}

int Kermit::spack(char type, int n, const char* data, int len)
{
    int i = 0;
    data_[i++] = MARK;
    data_[i++] = tochar(len + 3);
    data_[i++] = tochar(n);
    data_[i++] = type;
    for(int j = 0; j < len; j++)
        data_[i++] = *data++;
    data_[i] = '\0';
    data_[i++] = tochar(check(data_ + 1));
    data_[i++] = eol;
    send_packet(data_, i);
    return i;
}
