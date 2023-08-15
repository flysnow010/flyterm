#include "sshsession.h"
#include "util/util.h"
#include "child/sshwidget.h"
#include "dialog/connectdialog.h"
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QApplication>
#include <QStyleFactory>

SshSession::SshSession(QString const& name)
    : Session(name)
{
}

SshSession::SshSession(QString const& name, SSHSettings const& settings)
    : Session(name)
    , settings_(settings)
{
}

QString SshSession::type() const
{
    return QString("SshSession");
}

QIcon SshSession::icon()
{
    return QIcon(":image/Term/ssh.png");
}

void SshSession::edit()
{
    ConnectDialog dlg;
    dlg.setType(ConnectType::SSH);
    dlg.setSshSettings(settings_);
    if(dlg.exec() == QDialog::Accepted)
        settings_ = dlg.sshSettings();
}

bool SshSession::createShell(QMdiArea *midArea, bool isLog)
{
    SShWidget* widget = new SShWidget(isLog);
    QMdiSubWindow* subWindow = midArea->addSubWindow(widget);
    subWindow->setWindowTitle(QString("%1.%2").arg(index++).arg(name()));
    subWindow->setWindowIcon(icon());
    subWindow->setStyle(QStyleFactory::create("Fusion"));
    subWindow->setOption(QMdiSubWindow::RubberBandResize);
    subWindow->setOption(QMdiSubWindow::RubberBandMove);
    connect(widget, &SShWidget::onClose, this, &Session::onClose);
    connect(widget, &SShWidget::onSizeChanged, this, &Session::onSizeChanged);
    connect(widget, &SShWidget::onCommand, this, &Session::onCommand);
    connect(widget, &SShWidget::fontSizeChanged, this, &Session::fontSizeChanged);
    connect(widget, &SShWidget::highLighterChanged, this, &Session::highLighterChanged);
    connect(widget, &SShWidget::getHighlighter, this, &SshSession::setHighLighter);
    connect(subWindow, &QMdiSubWindow::windowStateChanged, this, &Session::windowStateChanged);

    ConsoleColor color = ConsoleColorManager::Instance()->color(colorIndex());
    ConsolePaletteManager* manager = ConsolePaletteManager::Instance();
    ConsolePalette::Ptr palette = manager->findPalette(paletteName());

    widget->setFontName(fontName());
    widget->setFontSize(fontSize());
    widget->setConsoleColor(color);
    if(palette)
        widget->setConsolePalette(palette);
    widget->updateHightLighter(hightLighter());
    widget->showMaximized();

    widgets_ << widget;
    return true;
}

void SshSession::setHighLighter()
{
    for(int i = 0; i < widgets_.size(); i++)
        widgets_[i]->setHighLighter(hightLighter());
}

void SshSession::updateTitle(QString const& name)
{
    for(int i = 0; i < widgets_.size(); i++)
    {
       QWidget* subWindow =  widgets_[i]->parentWidget();
       if(subWindow)
       {
           QString title = subWindow->windowTitle();
           int count = title.indexOf(QChar('.')) + 1;
           subWindow->setWindowTitle(QString("%1%2")
                                     .arg(title.left(count), name));
       }
    }
}

bool SshSession::runShell()
{
    if(widgets_.isEmpty())
        return false;
    return widgets_.last()->runShell(settings_);
}

void SshSession::closeSheel(QWidget *widget)
{
    widgets_.removeAll(dynamic_cast<SShWidget *>(widget));
}

void SshSession::sendCommand(QWidget *widget, QString const& command)
{
    SShWidget *theWidget = dynamic_cast<SShWidget *>(widget);
    if(theWidget)
        theWidget->sendCommand(command);
}

void SshSession::disconnect(QWidget *widget)
{
    SShWidget *theWidget = dynamic_cast<SShWidget *>(widget);
    if(theWidget)
        theWidget->disconnect();
}
void SshSession::save(QWidget *widget)
{
    SShWidget *theWidget = dynamic_cast<SShWidget *>(widget);
    if(theWidget)
        theWidget->save();
}
void SshSession::print(QWidget *widget)
{
    SShWidget *theWidget = dynamic_cast<SShWidget *>(widget);
    if(theWidget)
        theWidget->print();
}
void SshSession::copy(QWidget *widget)
{
    SShWidget *theWidget = dynamic_cast<SShWidget *>(widget);
    if(theWidget)
        theWidget->copy();
}

void SshSession::copyAll(QWidget *widget)
{
    SShWidget *theWidget = dynamic_cast<SShWidget *>(widget);
    if(theWidget)
        theWidget->copyAll();
}

void SshSession::paste(QWidget *widget)
{
    SShWidget *theWidget = dynamic_cast<SShWidget *>(widget);
    if(theWidget)
        theWidget->paste();
}

void SshSession::updateHightLighter(QString const& hightLighter)
{
    for(int i = 0; i < widgets_.size(); i++)
        widgets_[i]->updateHightLighter(hightLighter);
}

void SshSession::updateFontName(QString const& fontName)
{
    for(int i = 0; i < widgets_.size(); i++)
        widgets_[i]->setFontName(fontName);
}

void SshSession::updateColorIndex(int index)
{
    ConsoleColor color = ConsoleColorManager::Instance()->color(index);
    for(int i = 0; i < widgets_.size(); i++)
        widgets_[i]->setConsoleColor(color);
}

void SshSession::updatePaletteName(QString const& paletteName)
{
    ConsolePaletteManager* manager = ConsolePaletteManager::Instance();
    ConsolePalette::Ptr palette = manager->findPalette(paletteName);
    if(palette)
    {
        for(int i = 0; i < widgets_.size(); i++)
            widgets_[i]->setConsolePalette(palette);
    }
}

void SshSession::updateFontSize(int fontSize)
{
    for(int i = 0; i < widgets_.size(); i++)
        widgets_[i]->setFontSize(fontSize);
}

void SshSession::increaseFontSize(QWidget *widget)
{
    SShWidget *theWidget = dynamic_cast<SShWidget *>(widget);
    if(theWidget)
        theWidget->increaseFontSize();
}

void SshSession::decreaseFontSize(QWidget *widget)
{
    SShWidget *theWidget = dynamic_cast<SShWidget *>(widget);
    if(theWidget)
        theWidget->decreaseFontSize();
}

void SshSession::clearScrollback(QWidget *widget)
{
    SShWidget *theWidget = dynamic_cast<SShWidget *>(widget);
    if(theWidget)
        theWidget->clearScrollback();
}

bool SshSession::isThisWidget(QWidget *widget)
{
    int index = widgets_.indexOf(dynamic_cast<SShWidget *>(widget));
    return index != -1;
}

int  SshSession::shellSize()
{
    return widgets_.size();
}

void SshSession::activeWidget(QWidget *widget)
{
    for(int i = 0; i < widgets_.size(); i++)
    {
        if(widgets_[i] == widget)
        {
            widgets_[i]->activedWidget();
            break;
        }
    }
}

void SshSession::setObject(QJsonObject const& obj)
{
    settings_.hostName = obj.value("hostName").toString();
    settings_.userName = obj.value("userName").toString();
    settings_.port = obj.value("port").toInt();
    settings_.usePrivateKey = obj.value("usePrivateKey").toBool();
    settings_.privateKeyFileName = obj.value("privateKeyFileName").toString();
}

QJsonObject SshSession::object() const
{
    QJsonObject obj;

    obj.insert("hostName", settings_.hostName);
    obj.insert("userName", settings_.userName);
    obj.insert("port", settings_.port);
    obj.insert("usePrivateKey", settings_.usePrivateKey);
    obj.insert("privateKeyFileName", settings_.privateKeyFileName);

    return obj;
}
