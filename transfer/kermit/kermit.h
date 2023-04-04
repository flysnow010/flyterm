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

class Kermit
{
public:
    Kermit();

    enum Code {
        NUL = 0x00,
        MARK = 0x01,
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
        MinLen = 3,
        MaxLen = 94,
        MaxSize = MaxLen + 6
    };

    enum State {
        SSNUL = 0,
        SSINT = 1,
        SSFIL = 2,
        SSDAT = 3,
        SSEND = 4,
        SSBRK = 5
    };

protected:
    virtual void on_init(int seq, const char* data, int size);
    virtual void on_file_header(int seq, const char* data, int size);
    virtual void on_data(int seq, const char* data, int size);
    virtual void on_end(int seq, const char* data, int size);
    virtual void on_break(int seq, const char* data, int size);
    virtual void on_ack(int seq, const char* data, int size);
    virtual void on_nack(int seq, const char* data, int size);
    virtual void on_error(int seq, const char* data, int size);

    virtual int write(char const *data, int size) = 0;
    virtual int read(char *data, int size) = 0;
    virtual char getc() = 0;

protected:
    void send_init();
    void send_data(int n, const char* data, int len);
    void send_end(int n);
    void send_break(int n);
    void send_ack(int n);
    void send_nack(int n);

    bool recv_packet();
    void resend();

    int encode(char a, char* data);
    int decode(const char* data, char& a);
private:
    int tochar(int x) { return x + 32; }
    int unchar(int x) { return int(x - 32); }
    int ctl(int x) { return x^64; }
    int check(const char* p);
    int check(int sum, const char* begin, const char* end);
    int spack(char type, int n, const char* data, int len);
    bool send_packet(const char* data, int size);
private:
    char data_[MaxSize];
    int maxl = MaxLen;
    int time = 10;
    int npad = 0;
    int padc = 64;
    int eol = END_CHAR;
    char qctl = '#';
    int last_size = 0;
};

#endif // KERMIT_H
