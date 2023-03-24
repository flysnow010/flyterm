#ifndef TFTPSERVERFILE_H
#define TFTPSERVERFILE_H
#include "tftp.h"

#include <memory>
#include <fstream>
#include <map>

class BaseUdp;
class TFtpServerFile : public TFtp
{
public:
    TFtpServerFile(BaseUdp *udp, std::string const& path, std::string const& id)
        : udp_(udp)
        , type_(None)
        , file_path_(path)
        , transfer_id_(id)
        , block_number_(0)
    {}

    ~TFtpServerFile();

    using Ptr = std::shared_ptr<TFtpServerFile>;

    std::string transfer_id() const { return transfer_id_; }
    Type type() const { return type_; }
    std::string filename() const { return filename_; }
    uint16_t block_number() const { return block_number_; }
    uint16_t block_numbers() const { return static_cast<uint16_t>((filesize_ + BLOCK_SIZE - 1) / BLOCK_SIZE); }
    size_t filesize() const { return filesize_; }
    size_t file_bytes() const { return file_bytes_; }
protected:
    void on_read_req(std::string const& filename, Mode mode) override;
    void on_write_req(std::string const& filename, Mode mode) override;
    void on_data(uint16_t block_number, uint8_t const* data, uint32_t size) override;
    void on_ack(uint16_t block_number) override;
    void on_error(uint16_t error, std::string const& error_msg) override;
    uint32_t write(uint8_t const *data, size_t size) override;
private:
    void send_data(uint16_t block_number);
    std::string full_fileaname(std::string const& filename) const {
        return file_path_ + filename;
    }

    TFtpServerFile(TFtpServerFile const&);
    TFtpServerFile(TFtpServerFile &&);
    TFtpServerFile operator == (TFtpServerFile const&);
    TFtpServerFile operator == (TFtpServerFile &&);
private:
    BaseUdp* udp_;
    Type type_;
    std::string filename_;
    std::string file_path_;
    std::string transfer_id_;
    std::ifstream read_file;
    std::ofstream write_file;
    uint16_t block_number_;
    uint32_t block_size_ = 0;
    size_t filesize_ = 0;
    size_t file_bytes_ = 0;
};

class TFtpFileManager
{
public:
    TFtpFileManager() {}
    using Ptr = std::shared_ptr<TFtpFileManager>;

    size_t size() const { return tftpFiles_.size(); }
    TFtpServerFile::Ptr find(std::string const& transferId)
    {
        auto it = tftpFiles_.find(transferId);
        if(it != tftpFiles_.end())
            return it->second;
        return TFtpServerFile::Ptr();
    }

    void add(TFtpServerFile::Ptr const& file) { tftpFiles_[file->transfer_id()] = file; }
    void remove(std::string const& transfer_id)
    {
        auto it = tftpFiles_.find(transfer_id);
        if(it != tftpFiles_.end())
            tftpFiles_.erase(it);
    }
private:
    std::map<std::string, TFtpServerFile::Ptr> tftpFiles_;
};

#endif // TFTPSERVERFILE_H
