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
    void stop();
signals:
    void onCommand(QString const& command);
    void onExpandCommand(QString const& command);
    void onTestCommand(QString const& command);
    void onAllCommand(QString const& command);
protected:
    virtual void run();
private:
    QString command();
    void execCommand(QString const& command);
private:
    QStringList commands_;
    QMutex mutex_;
    QSemaphore sem_;
};

#endif // COMMANDTHREAD_H
