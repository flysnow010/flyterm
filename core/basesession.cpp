#include "basesession.h"
#include "sshsession.h"
#include "telnetsession.h"
#include "serialsession.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QMdiSubWindow>
#include <QMdiArea>
#include <QMenu>

Session::Session(QString const& name)
    : name_(name)
    , hightLighter_("errorHighLighter")
    , codecName_("UTF-8")
    , fontName_("Courier New")
    , colorIndex_(4)//"Black White
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

void Session::setCodecName(QString const& codecName)
{
    codecName_ = codecName;
    updateCodecName(codecName);
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

void Session::setColorIndex(int index)
{
    colorIndex_ = index;
    updateColorIndex(index);
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

void Session::recvFile(QWidget *widget, QString const& protocol)
{
    Q_UNUSED(widget)
    Q_UNUSED(protocol)
}

QMenu* Session::createSystemMenu(QMdiSubWindow *parent, QWidget *widget)
{
    QMenu* menu = new QMenu(parent);
    menu->addAction(QIcon(":image/File/save.png"), tr("Save Text"), this, [=]{
        save(widget);
    });
    menu->addAction(tr("Floating"), this, [=]{
        parent->mdiArea()->removeSubWindow(parent);
        parent->setWindowIcon(QIcon(":image/app.png"));
        parent->showMaximized();
    });
    menu->addSeparator();
    menu->addAction(tr("Close left tab"), this, [=]{
        QList<QMdiSubWindow *> widgets = parent->mdiArea()->subWindowList();
        int index = widgets.indexOf(parent);
        for(int i = 0; i < index; i++)
            widgets[i]->close();
    });
    menu->addAction(tr("Close right tab"), this, [=]{
        QList<QMdiSubWindow *> widgets = parent->mdiArea()->subWindowList();
        int index = widgets.indexOf(parent);
        if(index < 0)
            return;
        for(int i = index + 1; i < widgets.size(); i++)
            widgets[i]->close();
    });
    menu->addAction(tr("Close other tab"), this, [=]{
        QList<QMdiSubWindow *> widgets = parent->mdiArea()->subWindowList();
        for(int i = 0; i < widgets.size(); i++)
        {
            if(widgets[i] != parent)
                widgets[i]->close();
        }
    });
    menu->addAction(QIcon(":image/Term/close.png"), tr("Close"), this, [=]{
        parent->close();
    });
    return menu;
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
        session.insert("codecName", sessions_[i]->codecName());
        session.insert("fontName", sessions_[i]->fontName());
        session.insert("fontSize", sessions_[i]->fontSize());
        session.insert("colorIndex", sessions_[i]->colorIndex());
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
    int colorIndex = obj.value("colorIndex").toInt();
    QString paletteName = obj.value("paletteName").toString();
    QString codecName = obj.value("codecName").toString();
    QString fontName = obj.value("fontName").toString();
    int fontSize = obj.value("fontSize").toInt();

    if(type == "SshSession")
        session = Session::Ptr(new SshSession(name));
    else if(type == "TelnetSession")
        session = Session::Ptr(new TelnetSession(name));
    else if(type == "SerialSession")
        session = Session::Ptr(new SerialSession(name));

    if(session)
    {
        session->setHightLighter(hightLighter);
        if(!codecName.isEmpty())
            session->setCodecName(codecName);
        session->setFontName(fontName);
        session->setFontSize(fontSize);
        session->setColorIndex(colorIndex);
        if(!paletteName.isEmpty())
            session->setPaletteName(paletteName);
        session->setObject(obj.value("session").toObject());
    }

    return session;
}
