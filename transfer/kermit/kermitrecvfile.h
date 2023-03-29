#ifndef KERMITRECVFILE_H
#define KERMITRECVFILE_H

#include <QObject>

class KermitRecvFile : public QObject
{
    Q_OBJECT
public:
    explicit KermitRecvFile(QObject *parent = nullptr);

signals:

};

#endif // KERMITRECVFILE_H
