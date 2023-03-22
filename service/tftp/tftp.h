#ifndef TFTP_H
#define TFTP_H
#include <cstdint>
#include <string>

class TFtp
{
public:
    TFtp();

    enum Code {
        RRQ   = 0x0001,//Read request
        WRQ   = 0x0002,//Write request
        DATA  = 0x0003,//Data request
        ACK   = 0x0004,//Acknowledgement
        ERROR = 0x0005 //Error
    };
    enum Mode {
        BINARY,
        ASCII,
        MAIL
    };

    enum Error {
        NotDefined        = 0x0000,
        FileNotFound      = 0x0001,
        AccessViolation   = 0x0002,
        DiskFull          = 0x0003,
        IllegalOperation  = 0x0004,
        UnknownTransferID = 0x0005,
        FileExists        = 0x0006,
        NoSuchUser        = 0x0007,
    };

    enum Size {
        CODE_SIZE = 2,
        HEADER_SIZE = 4,
        BLOCK_SIZE  = 512
    };
    enum Type {
        None,
        Read,
        Write
    };

    bool process(uint8_t const *data, uint32_t size);
    bool is_finished() const { return finished_; }
    bool is_error() const { return !error_msg_.empty(); }
    Error error() const { return error_; }
    std::string error_msg() const { return error_msg_; }
protected:
    virtual void on_read_req(std::string const& filename, Mode mode);
    virtual void on_write_req(std::string const& filename, Mode mode);
    virtual void on_data(uint16_t block_number, uint8_t const*data, uint32_t size) = 0;
    virtual void on_ack(uint16_t block_number) = 0;
    virtual void on_error(uint16_t error, std::string const& error_msg) = 0;

    virtual uint32_t write(uint8_t const *data, size_t size) = 0;

    void read_req(std::string const& filename, Mode mode);
    void write_req(std::string const& filename, Mode mode);
    void send(uint16_t block_number, size_t size);
    void resend();
    void ack(uint16_t block_number);
    void error(Error error, std::string const& error_msg);
protected:
    char *data() { return (char *)(data_ + HEADER_SIZE); }
    void set_error(Error error, std::string const& error_msg)
    {
        error_ = error;
        error_msg_ = error_msg;
    }
    void finished() { finished_ = true; }
    size_t get_filesize(const char*filename);
private:
    uint16_t op_code(uint8_t const *data) { return static_cast<uint16_t>((data[0] << 8) | data[1]); }
    uint16_t block_num(uint8_t const *data) { return static_cast<uint16_t>((data[2] << 8) | data[3]); }
    uint16_t error_code(uint8_t const *data) { return static_cast<uint16_t>((data[2] << 8) | data[3]); }
    Mode getMode(std::string const& text);
    std::string getText(Mode mode);
private:
    uint8_t data_[HEADER_SIZE + BLOCK_SIZE];
    bool finished_ = false;
    TFtp::Error error_ = NotDefined;
    std::string error_msg_;
    size_t block_size_ = 0;
};

#endif // TFTP_H
