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
        NUL = 0x00,
        SOH  = 0x01,
        MARK = SOH,
        END_CHAR = 0x0D,
        MAX = 0xFF
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
        MinSize = 3,
        Size1 = 24
    };

protected:
    virtual void on_init(int seq, const char* data, uint32_t size);
    virtual void on_file_header(int seq, const char* data, uint32_t size);
    virtual void on_data(int seq, const char* data, uint32_t size);
    virtual void on_end(int seq, const char* data, uint32_t size);
    virtual void on_ack(int seq, const char* data, uint32_t size);
    virtual void on_nack(int seq, const char* data, uint32_t size);
    virtual void on_error(int seq, const char* data, uint32_t size);

    virtual uint32_t write(char const *data, uint32_t size) = 0;
    virtual uint32_t read(char *data, uint32_t size) = 0;
    virtual char getch() = 0;

protected:
    void send_init();
    void send_data(const char* data, uint32_t size);
    void send_ack(int n);
    void send_nack(int n);

    bool send_packet(const char* data, uint32_t size);
    bool recv_packet();

    uint8_t tochar(char x) { return x + 32; }
    int unchar(char x) { return int(x - 32); }
    uint8_t ctl(char x) { return x^64; }
    uint16_t check(const char* p);
    uint16_t check(uint16_t sum, const char* begin, const char* end);

    char ktrans(char in);
private:
    char data_[Size1];
    int maxl = 94;
    int time = 10;
    int npad = 0;
    int padc = 64;
    int eol = END_CHAR;
    char qctl = '#';
};

#endif // KERMIT_H
