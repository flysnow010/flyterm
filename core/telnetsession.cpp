#include "telnetsession.h"
#include "child/telnetwidget.h"
#include "dialog/connectdialog.h"

#include <QMdiArea>
#include <QMdiSubWindow>
#include <QApplication>

TelnetSession::TelnetSession(QString const& name)
    : Session(name)
{
}

TelnetSession::TelnetSession(QString const& name, TelnetSettings const& settings)
    : Session(name)
    , settings_(settings)
{
}

QString TelnetSession::type() const
{
    return QString("TelnetSession");
}

QIcon TelnetSession::icon()
{
    return QIcon(":image/Term/telnet.png");
}

void TelnetSession::edit()
{
    ConnectDialog dlg;
    dlg.setType(ConnectType::Telnet);
    dlg.setTelnetSettings(settings_);
    if(dlg.exec() == QDialog::Accepted)
        settings_ = dlg.telnetSettings();
}

bool TelnetSession::createShell(QMdiArea *midArea, bool isLog)
{
    TelnetWidget* widget = new TelnetWidget(isLog);
    QMdiSubWindow* subWindow = midArea->addSubWindow(widget);

    subWindow->setWindowTitle(QString("%1.%2").arg(index++).arg(name()));
    subWindow->setWindowIcon(icon());
    connect(widget, &TelnetWidget::onClose, this, &Session::onClose);
    connect(widget, &TelnetWidget::onCommand, this, &Session::onCommand);
    connect(widget, &TelnetWidget::fontSizeChanged, this, &Session::fontSizeChanged);
    connect(widget, &TelnetWidget::highLighterChanged, this, &Session::highLighterChanged);
    connect(widget, &TelnetWidget::getHighlighter, this, &TelnetSession::setHighLighter);
    connect(subWindow, &QMdiSubWindow::windowStateChanged, this, &Session::windowStateChanged);
    subWindow->setOption(QMdiSubWindow::RubberBandResize);
    subWindow->setOption(QMdiSubWindow::RubberBandMove);

    ConsoleColor color = ConsoleColorManager::Instance()->color(colorName());
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

void TelnetSession::setHighLighter()
{
    for(int i = 0; i < widgets_.size(); i++)
        widgets_[i]->setHighLighter(hightLighter());
}

void TelnetSession::updateTitle(QString const& name)
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

bool TelnetSession::runShell()
{
    if(widgets_.isEmpty())
        return false;
    return widgets_.last()->runShell(settings_);
}

void TelnetSession::sendCommand(QWidget *widget, QString const& command)
{
    TelnetWidget *theWidget = dynamic_cast<TelnetWidget *>(widget);
    if(theWidget)
        theWidget->sendCommand(command);
}

void TelnetSession::closeSheel(QWidget *widget)
{
    widgets_.removeAll(dynamic_cast<TelnetWidget *>(widget));
}

void TelnetSession::disconnect(QWidget *widget)
{
    TelnetWidget *theWidget = dynamic_cast<TelnetWidget *>(widget);
    if(theWidget)
        theWidget->disconnect();
}
void TelnetSession::save(QWidget *widget)
{
    TelnetWidget *theWidget = dynamic_cast<TelnetWidget *>(widget);
    if(theWidget)
        theWidget->save();
}
void TelnetSession::print(QWidget *widget)
{
    TelnetWidget *theWidget = dynamic_cast<TelnetWidget *>(widget);
    if(theWidget)
        theWidget->print();
}
void TelnetSession::copy(QWidget *widget)
{
    TelnetWidget *theWidget = dynamic_cast<TelnetWidget *>(widget);
    if(theWidget)
        theWidget->copy();
}

void TelnetSession::copyAll(QWidget *widget)
{
    TelnetWidget *theWidget = dynamic_cast<TelnetWidget *>(widget);
    if(theWidget)
        theWidget->copyAll();
}

void TelnetSession::paste(QWidget *widget)
{
    TelnetWidget *theWidget = dynamic_cast<TelnetWidget *>(widget);
    if(theWidget)
        theWidget->paste();
}

void TelnetSession::updateHightLighter(QString const& hightLighter)
{
    for(int i = 0; i < widgets_.size(); i++)
        widgets_[i]->updateHightLighter(hightLighter);
}

void TelnetSession::updateFontName(QString const& fontName)
{
    for(int i = 0; i < widgets_.size(); i++)
        widgets_[i]->setFontName(fontName);
}

void TelnetSession::updateColorName(QString const& colorName)
{
    ConsoleColor color = ConsoleColorManager::Instance()->color(colorName);
    for(int i = 0; i < widgets_.size(); i++)
        widgets_[i]->setConsoleColor(color);
}

void TelnetSession::updatePaletteName(QString const& paletteName)
{
    ConsolePaletteManager* manager = ConsolePaletteManager::Instance();
    ConsolePalette::Ptr palette = manager->findPalette(paletteName);
    if(palette)
    {
        for(int i = 0; i < widgets_.size(); i++)
            widgets_[i]->setConsolePalette(palette);
    }
}

void TelnetSession::updateFontSize(int fontSize)
{
    for(int i = 0; i < widgets_.size(); i++)
        widgets_[i]->setFontSize(fontSize);
}

void TelnetSession::increaseFontSize(QWidget *widget)
{
    TelnetWidget *theWidget = dynamic_cast<TelnetWidget *>(widget);
    if(theWidget)
        theWidget->increaseFontSize();
}

void TelnetSession::decreaseFontSize(QWidget *widget)
{
    TelnetWidget *theWidget = dynamic_cast<TelnetWidget *>(widget);
    if(theWidget)
        theWidget->decreaseFontSize();
}

void TelnetSession::clearScrollback(QWidget *widget)
{
    TelnetWidget *theWidget = dynamic_cast<TelnetWidget *>(widget);
    if(theWidget)
        theWidget->clearScrollback();
}

bool TelnetSession::isThisWidget(QWidget *widget)
{
    int index = widgets_.indexOf(dynamic_cast<TelnetWidget *>(widget));
    return index != -1;
}

int  TelnetSession::shellSize()
{
    return widgets_.size();
}

void TelnetSession::setObject(QJsonObject const& obj)
{
    settings_.hostName = obj.value("hostName").toString();
    settings_.userName = obj.value("userName").toString();
    settings_.port = obj.value("port").toInt();
}

QJsonObject TelnetSession::object() const
{
    QJsonObject obj;

    obj.insert("hostName", settings_.hostName);
    obj.insert("userName", settings_.userName);
    obj.insert("port", settings_.port);

    return obj;
}
