#ifndef KERMITFILESENDER_H
#define KERMITFILESENDER_H

#include <QObject>

class KermitFileSender : public QObject
{
    Q_OBJECT
public:
    explicit KermitFileSender(QObject *parent = nullptr);

signals:

};

#endif // KERMITFILESENDER_H
