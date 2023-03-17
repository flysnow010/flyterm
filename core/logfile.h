#ifndef LOGFILE_H
#define LOGFILE_H
#include <QString>
#include <QFile>
#include <memory>

class LogFile
{
public:
    LogFile();
    LogFile(LogFile const&) = delete;
    LogFile &operator =(LogFile const&) = delete;

    using Ptr = std::shared_ptr<LogFile>;
    bool open(QString const& fileName);
    bool write(QString  const& text);
    bool write(QByteArray  const& data);
private:
     QFile file;
};

#endif // LOGFILE_H
