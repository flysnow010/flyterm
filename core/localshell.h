#ifndef LOCALSHELL_H
#define LOCALSHELL_H

#include <QObject>

class QProcess;
class LocalShell : public QObject
{
    Q_OBJECT
public:
    explicit LocalShell(QObject *parent = nullptr);

    void setWorkingDirectory(QString const& directory);
    void start(QString const& shell);
    void start(QString const& shell, QStringList const& params);
    void wait();
    void stop();

    void write(QByteArray const& data);
signals:
    void onData(QByteArray const& data);
private slots:
    void readyReadStdout();
    void readyReadStderr();
private:
    QProcess* process;
};

#endif // LOCALSHELL_H
