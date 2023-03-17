#ifndef PASSWORDSERVER_H
#define PASSWORDSERVER_H
#include <QObject>
#include <QString>
#include <QList>
#include <memory>

class QLocalServer;
class QLocalSocket;
struct Password
{
    QString prompt;
    QString password;

    using Ptr = std::shared_ptr<Password>;
    static QString encrypt(QString const& text);
    static QString hash(QString const& text);
};

class PasswordServer : public QObject
{
    Q_OBJECT
public:
    PasswordServer(QObject *parent = nullptr);

    bool run(QString const& serverName);

    Password::Ptr findPassword(QString const& prompt);
    void addPassword(Password::Ptr const& password);

    bool save();
    bool load();
signals:
    void newClient(QLocalSocket *client);

private slots:
    void newConnection();
    void readPrompt();
private:
    QLocalServer *server;
    QList<Password::Ptr> passwords_;
};

#endif // PASSWORDSERVER_H
