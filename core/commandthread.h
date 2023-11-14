#ifndef COMMANDTHREAD_H
#define COMMANDTHREAD_H

#include <QThread>
#include <QMutex>
#include <QSemaphore>
#include <QStringList>

class CommandThread : public QThread
{
     Q_OBJECT
public:
    explicit CommandThread(QObject *parent = nullptr);

    void postCommand(QString const& command);
    void runOrderCommand();
    void stop();
signals:
    void onCommand(QString const& command);
    void onExpandCommand(QString const& command);
    void onTestCommand(QString const& command);
    void onAllCommand(QString const& command);
    void onOrderCommandStart(QString const& runCommandFlag);
    void onOrderCommandEnd();
protected:
    virtual void run();
private:
    QString command();
    QString orderCommand();
    void pushOrderCommand(QString const& orderCommand);
    void execCommand(QString const& command);
    void execAppCommand(QString const& command);
    void execNextOrderCommand();

    bool singled() { return signal_; }
    void doSignal() { signal_ = true; };
private:
    volatile bool signal_ = false;
    QStringList commands_;
    QStringList orderCommands_;
    QMutex mutex_;
    QSemaphore sem_;
};

#endif // COMMANDTHREAD_H
