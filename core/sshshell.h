#ifndef SSHSHELL_H
#define SSHSHELL_H

#include <QObject>
#include <QThread>

class SSHChannel;
class SSHSettings;
class SshShell : public QObject
{
    Q_OBJECT
public:
    explicit SshShell(QObject *parent = nullptr);
    ~SshShell();

    void connectTo(SSHSettings const& settings);
    int write(QByteArray const& data);
    void shellSize(int cols, int rows);
    void run();
    void stop();

signals:
    void connectReq(SSHSettings const& settings);
    void runReq();
    void shellSizeReq(int cols, int rows);

    void connected();
    void unconnected();
    void onData(QByteArray const& data);
    void onError(QByteArray const& data);
    void connectionError(QString const& error);
private:
    QThread workerThread;
    SSHChannel* worker_;
};

#endif // SSHSHELL_H
