#include "util.h"

#include <QDir>
#include <QUuid>
#include <QApplication>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QtWin>

#include <cstring>
#include <cstdio>
#include <windows.h>

namespace
{
    QString theCachePath = QDir::homePath();
}

QString Util::currentPath()
{
    QString filePath = QApplication::applicationDirPath();
    if(filePath.startsWith("C:") ||filePath.startsWith("c:"))
        return QString("%1/FlyTerm").arg(QDir::homePath());
    return filePath;
}

QString Util::serverName()
{
    static QString theServerName;
    if(theServerName.isEmpty())
        theServerName = QUuid::createUuid().toString();
    return theServerName;
}

QString Util::plotAppPath()
{
    return QString("%1/FlyPlot.exe").arg(QApplication::applicationDirPath());
}

QString Util::homePath()
{
    return QString("%1/FlyTerm/V1.0.0").arg(QDir::homePath());
}

QString Util::sessionPath()
{
    QString path = QString("%1/Sessions").arg(currentPath());
    QDir dir(path);
    if(!dir.exists(path))
        dir.mkpath(path);
    return path;
}

QString Util::commandPath()
{
    QString path = QString("%1/Commands").arg(currentPath());
    QDir dir(path);
    if(!dir.exists(path))
        dir.mkpath(path);
    return path;
}

QString Util::sshUserAuthPath()
{
    QString path = QString("%1/sshUserAuth").arg(currentPath());
    QDir dir(path);
    if(!dir.exists(path))
        dir.mkpath(path);
    return path;
}

QString Util::languagePath()
{
    return QString("%1/languages").arg(QApplication::applicationDirPath());
}

QString Util::logPath()
{
    QString path = QString("%1/Logs").arg(currentPath());
    QDir dir(path);
    if(!dir.exists(path))
        dir.mkpath(path);
    return path;
}

QString Util::syscfgPath()
{
    QString path = QString("%1/FlyTerm/V1.0.0/syscfg").arg(QDir::homePath());
    QDir dir(path);
    if(!dir.exists(path))
        dir.mkpath(path);
    return path;
}

QString Util::cachePath()
{
    return theCachePath;
}

void Util::setCachePath(QString const& path)
{
    theCachePath = path;
}

QString Util::tempPath()
{
    QString path = QString("%1/FlyTerm/V1.0.0").arg(QDir::tempPath());
    QDir dir(path);
    if(!dir.exists(path))
        dir.mkpath(path);
    return path;
}

void Util::writeFile(QString const& fileName, QString const& data)
{
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
        return;
    file.write(data.toUtf8());
}

QByteArray Util::readFile(QString const& filename)
{
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly))
        return file.readAll();
    return QByteArray();
}

bool Util::parseJson(QString const& filename, QJsonDocument &doc)
{
    QByteArray bytes = readFile(filename);
    return parseJson(bytes, doc);
}

bool Util::parseJson(QByteArray const& bytes, QJsonDocument &doc)
{
    QJsonParseError json_error;
    doc = QJsonDocument::fromJson(bytes, &json_error);
    if(json_error.error == QJsonParseError::NoError)
        return true;
    return false;
}

qint64 const SIZE_KB = 0x400;
qint64 const SIZE_MB = 0x100000;
qint64 const SIZE_GB = 0x40000000;
qint64 const SIZE_TB = 0x10000000000;

bool Util::question(QString const& text)
{
    int button = QMessageBox::question(0, QApplication::applicationName(), text);
    return button == QMessageBox::Yes;
}

QIcon Util::GetIcon(QString const& fileName, int index)
{
    HICON hicon;
    if(ExtractIconEx(fileName.toStdWString().c_str(), index, &hicon, 0, 1) > 0)
    {
        if(hicon)
            return QIcon(QtWin::fromHICON(hicon));
    }
    return QIcon();
}

QIcon Util::getOsIcon(QString const& os)
{
    QStringList icons = QStringList()
            << "windows" << "ubuntu" << "debian" << "kali" << "oracle" << "suse";
    foreach(auto icon, icons)
    {
        if(os.contains(icon))
            return QIcon(QString(":image/Os/%1.png").arg(icon));
    }
    return QIcon(":image/Os/linux.png");
}

QString Util::fromUnicode(QByteArray const& data)
{
    return QString::fromUtf16(reinterpret_cast<const ushort *>(data.data()),
                       data.size() / sizeof(ushort));
}

QString Util::formatFileSize(qint64 byte)
{
    if(byte < SIZE_KB)
        return QString("%1 B").arg(byte);
    else if(byte >= SIZE_KB  && byte < SIZE_MB)
        return QString("%1 KB").arg(static_cast<double>(byte) / SIZE_KB, 0, 'f', 2);
    else if(byte >= SIZE_MB && byte < SIZE_GB)
        return QString("%1 MB").arg(static_cast<double>(byte) / SIZE_MB, 0, 'f', 2);
    else if(byte >= SIZE_GB && byte < SIZE_TB)
        return QString("%1 GB").arg(static_cast<double>(byte) / SIZE_GB, 0, 'f', 2);
    else
        return QString("%1 TB").arg(static_cast<double>(byte) / SIZE_TB, 0, 'f', 2);
}

QString Util::formatFileSizeB(qint64 byte)
{
    QStringList sizes;
    qint64 size = byte;
    while(size > 1000)
    {
        sizes.push_front(QString("%1").arg(size % 1000));
        size /= 1000;
    }
    if(size)
        sizes.push_front(QString("%1").arg(size));
    return sizes.join(",");
}

QString Util::formatFileSizeKB(qint64 byte)
{
    return QString("%1 KB").arg(formatFileSizeB((byte +SIZE_KB - 1) / SIZE_KB));
}

QString Util::getOpenFileName(QString const& caption)
{
    return QFileDialog::getOpenFileName(0, caption);
}

QString Util::getSaveFileName(QString const& caption)
{
    return QFileDialog::getSaveFileName(0, caption);
}

QString Util::getText(QString const& label, QString const& value)
{
    QInputDialog dialog;
    dialog.setInputMode(QInputDialog::TextInput);
    dialog.setWindowTitle(QApplication::applicationName());
    dialog.setLabelText(label);
    dialog.setTextValue(value);
    dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);
    if(dialog.exec() == QDialog::Accepted)
        return dialog.textValue();
    return QString();
}
