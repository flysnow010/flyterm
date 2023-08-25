#ifndef SHELLTHREAD_H
#define SHELLTHREAD_H

#include <QThread>
#include <QByteArrayList>
#include <QMutex>

class ConsoleParser;
class ShellThread : public QThread
{
    Q_OBJECT
public:
    explicit ShellThread(QObject *parent = nullptr);
    ~ShellThread();

    void setCommandParser(ConsoleParser* parser);
    void stop() { doSignal(); }

    bool getData(QByteArray &data);
public slots:
    void addData(const QByteArray &data);
signals:
    void onData(QByteArray const& data);
protected:
    virtual void run();
private:
    bool singled() { return signal_; }
    void doSignal() { signal_ = true; };
private:
    QByteArrayList byteArrayList;
    QMutex mutex;
    ConsoleParser* commandParser = 0;
    volatile bool signal_ = false;
};

#endif // SHELLTHREAD_H
