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

    using SharedPtr = std::shared_ptr<LogFile>;
    using WeakPtr = std::weak_ptr<LogFile>;
    bool open(QString const& fileName, bool isAppend = true);
    bool write(QString  const& text);
    bool write(QByteArray  const& data);
private:
     QFile file;
};

#endif // LOGFILE_H
