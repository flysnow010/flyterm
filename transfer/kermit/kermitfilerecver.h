#ifndef KERMITFILERECVER_H
#define KERMITFILERECVER_H

#include <QObject>

class KermitFileRecver : public QObject
{
    Q_OBJECT
public:
    explicit KermitFileRecver(QObject *parent = nullptr);

signals:

};

#endif // KERMITFILERECVER_H
