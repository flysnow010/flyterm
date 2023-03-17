#ifndef TFTPFILE_H
#define TFTPFILE_H
#include "tftp.h"

#include <memory>
#include <fstream>
#include <map>

class BaseUdp;
class TFtpFile : public TFtp
{
public:
    TFtpFile(BaseUdp *udp, std::string const& path, std::string const& id)
        : udp_(udp)
        , file_path_(path)
        , transfer_id_(id)
        , type_(None)
        , block_number_(0)
    {}

    ~TFtpFile();

    using Ptr = std::shared_ptr<TFtpFile>;

    enum Type { None, Read, Write };

    std::string transfer_id() const { return transfer_id_; }
    Type type() const { return type_; }
    std::string filename() const { return filename_; }
    uint16_t block_number() const { return block_number_; }
    uint16_t block_numbers() const { return (filesize_ + BLOCK_SIZE - 1) / BLOCK_SIZE; }
    size_t filesize() const { return filesize_; }
protected:
    void on_read_req(std::string const& filename, Mode mode) override;
    void on_write_req(std::string const& filename, Mode mode) override;
    void on_data(uint16_t block_number, uint8_t const* data, uint32_t size) override;
    void on_ack(uint16_t block_number) override;
    void on_error(uint16_t error, std::string const& error_msg) override;

    uint32_t write(uint8_t const *data, uint32_t size) override;
private:
    void send_data(uint16_t block_number);
    std::string full_fileaname(std::string const& filename) const {
        return file_path_ + filename;
    }
    size_t get_filesize(const char*filename);

    TFtpFile(TFtpFile const&);
    TFtpFile(TFtpFile &&);
    TFtpFile operator == (TFtpFile const&);
    TFtpFile operator == (TFtpFile &&);
private:
    BaseUdp* udp_;
    std::string filename_;
    std::string file_path_;
    size_t filesize_ = 0;
    std::string transfer_id_;
    Type type_;
    std::ifstream read_file;
    std::ofstream write_file;
    uint16_t block_number_;

};

class TFtpFileManager
{
public:
    TFtpFileManager() {}
    using Ptr = std::shared_ptr<TFtpFileManager>;

    int size() const { return tftpFiles_.size(); }
    TFtpFile::Ptr find(std::string const& transferId)
    {
        auto it = tftpFiles_.find(transferId);
        if(it != tftpFiles_.end())
            return it->second;
        return TFtpFile::Ptr();
    }

    void add(TFtpFile::Ptr const& file) { tftpFiles_[file->transfer_id()] = file; }
    void remove(std::string const& transfer_id)
    {
        auto it = tftpFiles_.find(transfer_id);
        if(it != tftpFiles_.end())
            tftpFiles_.erase(it);
    }
private:
    std::map<std::string, TFtpFile::Ptr> tftpFiles_;
};

#endif // TFTPFILE_H
