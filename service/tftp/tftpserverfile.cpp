#include "tftpserverfile.h"
#include "baseudp.h"

#include <iostream>

TFtpServerFile::~TFtpServerFile() { delete udp_; }

void TFtpServerFile::on_read_req(std::string const& filename, Mode mode)//read
{
    if(type_ != None)
    {
        error(IllegalOperation, "Illegal TFTP Operation in RRQ");
        return;
    }

    type_ = Read;
    filename_ = full_fileaname(filename);

    if(mode == TFtp::BINARY)
        read_file.open(filename_.c_str(),
                       std::ifstream::in | std::ifstream::binary);
    else
        read_file.open(filename_.c_str());

    if(!read_file.is_open())
        error(FileNotFound,  std::string("File(") + filename + std::string(") Not Found"));
    else
    {
        block_number_ = 1;
        filesize_ = get_filesize(filename_.c_str());
        send_data(block_number_);
    }
}

void TFtpServerFile::on_write_req(std::string const& filename, Mode mode)//write
{
    if(type_ != None)
    {
        error(IllegalOperation, "Illegal TFTP Operation in WRQ");
        return;
    }

    type_ = Write;
    filename_ = full_fileaname(filename);

    if(mode == TFtp::BINARY)
        write_file.open(filename_.c_str(),
                        std::ifstream::out | std::ifstream::binary);
    else
        write_file.open(filename_.c_str());

    if(!write_file.is_open())
        error(AccessViolation, "Access Violation");
    else
        ack(block_number_);//ack(0)
}

void TFtpServerFile::on_data(uint16_t block_number, uint8_t const* data, uint32_t size) //write
{
    if(type_ != Write)
    {
        error(IllegalOperation, "Illegal TFTP Operation in Data");
        return;
    }

    if(block_number != block_number_ + 1)
        ack(block_number_);
    else
    {
        write_file.write((char *)data, size);
        file_bytes_ += size;
        ack(block_number);
        block_number_ = block_number;
        if(size < BLOCK_SIZE)
        {
            filesize_ = file_bytes_;
            write_file.close();
            finished();
        }
    }
}

void TFtpServerFile::on_ack(uint16_t block_number) // read
{
    if(type_ != Read)
    {
        error(IllegalOperation, "Illegal TFTP Operation in ACK");
        return;
    }

    if(read_file.eof())
    {
        finished();
        return;
    }

    if(block_number != block_number_)
        resend();
    else
    {
        block_number_++;
        send_data(block_number_);
    }
}

void TFtpServerFile::send_data(uint16_t block_number)
{
    char* d = data();

    read_file.read(d, TFtp::BLOCK_SIZE);
    file_bytes_ += read_file.gcount();
    send(block_number, read_file.gcount());
}

void TFtpServerFile::on_error(uint16_t error, std::string const& error_msg) //read/write
{
    set_error((Error)error, error_msg + std::string(" come from remote"));
    finished();
}

uint32_t TFtpServerFile::write(uint8_t const *data, size_t size)
{
    return udp_->write((const char*)data, size);
}
