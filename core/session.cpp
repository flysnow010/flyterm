#include "session.h"
#include "sshsession.h"
#include "telnetsession.h"
#include "serialsession.h"

#include <QJsonArray>
#include <QJsonDocument>

Session::Session(QString const& name)
    : name_(name)
    , hightLighter_("errorHighLighter")
    , fontName_("Courier New")
    , colorName_("Black White")
    , paletteName_("XTerm")

    , fontSize_(12)
{
    connect(this, &Session::fontSizeChanged, [this](int fonstSize){
        fontSize_ = fonstSize;
    });
}

void Session::setName(QString const& name)
{
    name_ = name;
    updateTitle(name);
}

void Session::setHightLighter(QString const& hightLighter)
{
    hightLighter_ = hightLighter;
    updateHightLighter(hightLighter);
}

void Session::setFontName(QString const& fontName)
{
    fontName_ = fontName;
    updateFontName(fontName);
}

void Session::setFontSize(int fontSize)
{
    fontSize_ = fontSize;
    updateFontSize(fontSize);
}

void Session::setColorName(QString const& name)
{
    colorName_ = name;
    updateColorName(colorName_);
}

void Session::setPaletteName(QString const& name)
{
    paletteName_ = name;
    updatePaletteName(name);
}

void Session::sendFile(QWidget *widget, QString const& protocol)
{
    Q_UNUSED(widget)
    Q_UNUSED(protocol)
}

SessionManager::SessionManager()
{
}

int SessionManager::size() const
{
    return sessions_.size();
}

Session::Ptr SessionManager::session(int index)
{
    if(index >=0 && index < sessions_.size())
        return sessions_.at(index);
    return Session::Ptr();
}

int SessionManager::shellSize(int index) const
{
    return shellSizes_.at(index);
}

Session::Ptr SessionManager::findSession(QWidget *widget)
{
    for(int i = 0; i < sessions_.size(); i++)
    {
        if(sessions_[i]->isThisWidget(widget))
            return sessions_[i];
    }
    return Session::Ptr();
}

void SessionManager::addSession(Session::Ptr const& session)
{
    sessions_.append(session);
}

void SessionManager::removeSession(Session::Ptr const& session)
{
    sessions_.removeAll(session);
}

void SessionManager::upSession(int index)
{
    int newIndex = index - 1;
    if(newIndex >= 0 && index < sessions_.size())
    {
        Session::Ptr sesion = sessions_.at(newIndex);
        sessions_[newIndex] = sessions_.at(index);
        sessions_[index] = sesion;
    }
}

void SessionManager::downSession(int index)
{
    int newIndex = index + 1;
    if(newIndex > 0 && newIndex < sessions_.size())
    {
        Session::Ptr sesion = sessions_.at(newIndex);
        sessions_[newIndex] = sessions_.at(index);
        sessions_[index] = sesion;
    }
}

bool SessionManager::save(QString const& fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
        return false;

    QJsonArray sessions;
    for(int i = 0; i < sessions_.size(); i++)
    {
        QJsonObject session;
        session.insert("type", sessions_[i]->type());
        session.insert("name", sessions_[i]->name());
        session.insert("hightLighter", sessions_[i]->hightLighter());
        session.insert("fontName", sessions_[i]->fontName());
        session.insert("fontSize", sessions_[i]->fontSize());
        session.insert("colorName", sessions_[i]->colorName());
        session.insert("paletteName", sessions_[i]->paletteName());
        session.insert("shellSize", sessions_[i]->shellSize());
        session.insert("session", sessions_[i]->object());
        sessions.append(session);
    }

    QJsonObject sessionManager;
    sessionManager.insert("version", "1.0");
    sessionManager.insert("sessions", sessions);

    QJsonDocument doc;
    doc.setObject(sessionManager);
    file.write(doc.toJson());

    return true;
}

bool SessionManager::load(QString const& fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
        return false;

    QJsonParseError json_error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &json_error);
    if(json_error.error != QJsonParseError::NoError)
        return false;

    QJsonObject sessionManager = doc.object();
    QJsonArray sessions = sessionManager.value("sessions").toArray();
    for(int i = 0; i < sessions.size(); i++)
    {
        QJsonObject object = sessions.at(i).toObject();
        Session::Ptr session = createSession(object);
        if(session)
        {
            sessions_ << session;
            shellSizes_ << object.value("shellSize").toInt();
        }
    }

    return true;
}

Session::Ptr SessionManager::createSession(QJsonObject const& obj)
{
    Session::Ptr session;
    QString type = obj.value("type").toString();
    QString name = obj.value("name").toString();
    QString hightLighter = obj.value("hightLighter").toString();
    QString colorName = obj.value("colorName").toString();
    QString paletteName = obj.value("paletteName").toString();
    QString fontName = obj.value("fontName").toString();
    int fontSize = obj.value("fontSize").toInt();

    if(type == "SshSession")
    {
        session = Session::Ptr(new SshSession(name));
        session->setHightLighter(hightLighter);
        session->setFontName(fontName);
        session->setFontSize(fontSize);
        if(!colorName.isEmpty())
            session->setColorName(colorName);
        if(!paletteName.isEmpty())
            session->setPaletteName(paletteName);
        session->setObject(obj.value("session").toObject());
    }
    else if(type == "TelnetSession")
    {
        session = Session::Ptr(new TelnetSession(name));
        session->setHightLighter(hightLighter);
        session->setFontName(fontName);
        session->setFontSize(fontSize);
        if(!colorName.isEmpty())
            session->setColorName(colorName);
        if(!paletteName.isEmpty())
            session->setPaletteName(paletteName);
        session->setObject(obj.value("session").toObject());
    }
    else if(type == "SerialSession")
    {
        session = Session::Ptr(new SerialSession(name));
        session->setHightLighter(hightLighter);
        session->setFontName(fontName);
        session->setFontSize(fontSize);
        if(!colorName.isEmpty())
            session->setColorName(colorName);
        if(!paletteName.isEmpty())
            session->setPaletteName(paletteName);
        session->setObject(obj.value("session").toObject());
    }

    return session;
}
