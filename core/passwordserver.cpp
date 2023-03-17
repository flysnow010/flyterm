#include "passwordserver.h"
#include "util/util.h"
#include "crypto/cryptorsa.h"
#include <crypto/pubKey.h>
#include <qlocalserver.h>
#include <qlocalsocket.h>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QCryptographicHash>

QString Password::encrypt(QString const& text)
{
    QString publicKey = QString::fromUtf8(pubKey, sizeof(pubKey));
    return  CryptoRSA::EncryptString(publicKey, text);
}

QString Password::hash(QString const& text)
{
    return QString::fromUtf8(QCryptographicHash::hash(text.toUtf8(), QCryptographicHash::Md5).toBase64());
}

PasswordServer::PasswordServer(QObject *parent)
    : QObject(parent)
    , server(new QLocalServer(this))
{
    connect(server, &QLocalServer::newConnection, this, &PasswordServer::newConnection);
}

Password::Ptr PasswordServer::findPassword(QString const& prompt)
{
    for(int i = 0; i < passwords_.size(); i++)
    {
        if(passwords_[i]->prompt == prompt)
            return passwords_[i];
    }
    return Password::Ptr();
}

void PasswordServer::addPassword(Password::Ptr const& password)
{
    Password::Ptr p = findPassword(password->prompt);
    if(p)
        p->password = password->password;
    else
        passwords_ << password;
}

bool PasswordServer::run(QString const& serverName)
{
    return server->listen(serverName);
}

void PasswordServer::newConnection()
{
    QLocalSocket *client = server->nextPendingConnection();
    connect(client, &QLocalSocket::readyRead, this, &PasswordServer::readPrompt);
    connect(client, &QLocalSocket::disconnected, client, &QLocalSocket::deleteLater);
}

void PasswordServer::readPrompt()
{
    QLocalSocket *client = (QLocalSocket *)sender();
    emit newClient(client);
}

bool PasswordServer::save()
{
    QString fileName = QString("%1/Passwords.json").arg(Util::passwordPath());

    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
        return false;

    QJsonArray passwords;
    for(int i = 0; i < passwords_.size(); i++)
    {
        QJsonObject password;
        password.insert("prompt", passwords_[i]->prompt);
        password.insert("password", passwords_[i]->password);
        passwords.append(password);
    }

    QJsonObject passwordManager;
    passwordManager.insert("version", "1.0");
    passwordManager.insert("passwords", passwords);

    QJsonDocument doc;
    doc.setObject(passwordManager);
    file.write(doc.toJson());

    return true;
}

bool PasswordServer::load()
{
    QString fileName = QString("%1/Passwords.json").arg(Util::passwordPath());

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
        return false;

    QJsonParseError json_error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &json_error);
    if(json_error.error != QJsonParseError::NoError)
        return false;

    QJsonObject passwordManager = doc.object();
    QJsonArray passwords = passwordManager.value("passwords").toArray();
    for(int i = 0; i < passwords.size(); i++)
    {
        QJsonObject object = passwords.at(i).toObject();
        Password::Ptr password(new Password);
        password->prompt = object.value("prompt").toString();
        password->password = object.value("password").toString();
        passwords_ << password;
    }

    return true;
}

