#include "logfile.h"

LogFile::LogFile()
{
}

bool LogFile::open(QString const& fileName)
{
    file.setFileName(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Append))
        return false;
    return true;
}

bool LogFile::write(QString  const& text)
{
    file.write(text.toUtf8());
    return true;
}

bool LogFile::write(QByteArray  const& data)
{
    file.write(data);
    return true;
}
