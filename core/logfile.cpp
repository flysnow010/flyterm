#include "logfile.h"

LogFile::LogFile()
{
}

bool LogFile::open(QString const& fileName, bool isAppend)
{
    quint32 openFlag =  QIODevice::WriteOnly;
    if(isAppend)
        openFlag |= QIODevice::Append;

    file.setFileName(fileName);
    if(!file.open((QIODevice::OpenModeFlag)openFlag))
        return false;
    return true;
}

bool LogFile::write(QString  const& text)
{
    file.write(text.toUtf8());
    file.flush();
    return true;
}

bool LogFile::write(QByteArray  const& data)
{
    file.write(data);
    file.flush();
    return true;
}
