#ifndef LOCALSHELL_H
#define LOCALSHELL_H

#include <QObject>

class QProcess;
class LocalShell : public QObject
{
    Q_OBJECT
public:
    explicit LocalShell(QObject *parent = nullptr);

    void start(QString const& shell);
    void stop();

    void write(QByteArray const& data);
signals:
    void onData(QByteArray const& data);
private slots:
    void readyReadSto();
    void readyReadStr();
private:
    QProcess* process;
};

#endif // LOCALSHELL_H
