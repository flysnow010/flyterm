#ifndef SHELLTHREAD_H
#define SHELLTHREAD_H

#include <QThread>
#include <QByteArrayList>
#include <QMutex>

class ShellThread : public QThread
{
    Q_OBJECT
public:
    explicit ShellThread(QObject *parent = nullptr);
    ~ShellThread();

    void stop() { doSignal(); }
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
    volatile bool signal_ = false;
};

#endif // SHELLTHREAD_H
