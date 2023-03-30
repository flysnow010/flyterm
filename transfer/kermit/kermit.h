#ifndef KERMIT_H
#define KERMIT_H
/*
      |<------Included in CHECK----->|
      |                |
    +------+-----+-----+------+------+- - -+-------+
    | MARK | LEN | SEQ | TYPE | DATA | CHECK |<terminator>
    +------+-----+-----+------+------+- - -+-------+
             |            |
             |<--LEN-32 characters-->|

    MARK A real control character, usually CTRL-A.
    LEN One character, length of remainder of packet + 32, max 95
    SEQ One character, packet sequence number + 32, modulo 64
    TYPE One character, an uppercase letter
    CHECK One, two, or three characters, as negotiated.
    <terminator> Any control character required for reading the packet.
*/
#include <cstdint>

class Kermit
{
public:
    Kermit();

    enum Code {
        SOH  = 0x01,
        MARK = SOH,
        END_CHAR = 0x0D
    };

    enum Type {
        S = 0x53, //Send Initiation. I’m about to send files, and here are my parameters.
        F = 0x46, //File Header, the name of the file which is about to come.
        D = 0x44, //File Data
        Z = 0x5A, //End of File.
        B = 0x42, //Break Transmission, end of transaction.
        Y = 0x59, //Acknowledgment
        N = 0x4E, //Negative Acknowledgment
        E = 0x45  //Fatal Error
    };

    enum Size {
        Size1 = 24
    };

protected:
    virtual uint32_t write(char const *data, uint32_t size) = 0;
    virtual uint32_t read(char *data, uint32_t size) = 0;
protected:
    void start_send();
    void ack(int n);
    void nack(int n);
    void send_packe(const char* data, uint32_t size);
    uint8_t tochar(uint8_t x) { return x + 32; }
    uint8_t unchar(uint8_t x) { return x - 32; }
    uint8_t ctl(uint8_t x) { return x^64; }
    uint16_t check(const char* p);
    uint16_t checksum(const char* p);
private:
    char data_[Size1];
    char his_eol = END_CHAR;
    char maxl = 94;
    char time = 10;
    char npad = 0;
    char padc = 64;
    char eol = 13;
    char qctl = '#';
};

#endif // KERMIT_H
