#ifndef UTIL_H
#define UTIL_H
#include <QString>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDataStream>
#include <QTime>
class QFile;
class Util
{
public:
    static QString currentPath();
    static QString serverName();
    static QString tempPath();
    static QString syscfgPath();
    static QString homePath();
    static QString sessionPath();
    static QString commandPath();
    static QString sshUserAuthPath();
    static QString languagePath();
    static QString logoPath();
    static QString cachePath();
    static void setCachePath(QString const& path);
    static void writeFile(QString const& fileName, QString const& data);
    static QByteArray readFile(QString const& filename);
    static bool parseJson(QString const& filename, QJsonDocument &doc);
    static bool parseJson(QByteArray const& bytes, QJsonDocument &doc);
    static inline QJsonObject findObject(QJsonObject const& object,
                                         QString const& name)
    {
        QJsonObject::const_iterator it = object.find(name);
        if(it != object.constEnd() && it->isObject())
            return it->toObject();
        return QJsonObject();
    }
    static inline QJsonArray findArray(QJsonObject const& object,
                                       QString const& name)
    {
        QJsonObject::const_iterator it = object.find(name);
        if(it != object.constEnd() && it->isArray())
            return it->toArray();
        return QJsonArray();
    }

    static inline QString findString(QJsonObject const& object,
                                     QString const& name)
    {
        QJsonObject::const_iterator it = object.find(name);
        if(it != object.constEnd() && it->isString())
            return it->toString();
        return QString();
    }

    static inline QString objectToString(QJsonObject const& object)
    {
        QJsonDocument doc;
        doc.setObject(object);
        return QString::fromUtf8(doc.toJson());
    }
    static inline QJsonObject stringToObject(QString const& text)
    {
        QJsonDocument doc;
        if(parseJson(text.toUtf8(), doc))
            return doc.object();
        return QJsonObject();
    }

    static inline QString arrayToString(QJsonArray const& array)
    {
        QJsonDocument doc;
        doc.setArray(array);
        return QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
    }

    static inline void saveJsonObject(QDataStream & stream, QJsonObject const& obj)
    {
        QByteArray bytes = QJsonDocument(obj).toBinaryData();
        stream << bytes.size();
        stream.writeRawData(bytes.data(), bytes.size());
    }

    static inline void loadJsonObject(QDataStream & stream, QJsonObject & obj)
    {
        QByteArray bytes;
        int size = 0;
        stream >> size;
        bytes.resize(size);
        stream.readRawData(bytes.data(), bytes.size());
        QJsonDocument doc = QJsonDocument::fromBinaryData(bytes);
        obj = doc.object();
    }

    static QString formatFileSize(qint64 byte);
    static QString formatFileSizeB(qint64 byte);
    static QString formatFileSizeKB(qint64 byte);
    static QString formatFileSizeMB(qint64 byte);
    static QString getOpenFileName(QString const& caption);
    static QString getSaveFileName(QString const& caption);
    static QString getText(QString const& label, QString const& value = QString());
};

#endif // UTILS_H
