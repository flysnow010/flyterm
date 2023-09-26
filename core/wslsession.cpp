#include "wslsession.h"
#include "child/wslwidget.h"
#include "dialog/connectdialog.h"
#include "util/util.h"

#include <QMdiArea>
#include <QMenu>
#include <QMdiSubWindow>
#include <QApplication>
#include <QStyleFactory>

WSLSession::WSLSession(const QString &name)
    : Session(name)
{
}

WSLSession::WSLSession(QString const& name, WSLSettings const& settings)
    : Session(name)
    , settings_(settings)
{
}

QString WSLSession::type() const
{
    return QString("WSLSession");
}

QIcon WSLSession::icon()
{
    return Util::getOsIcon(settings_.distribution.toLower());
}

void WSLSession::edit()
{
    ConnectDialog dlg;
    dlg.setType(ConnectType::WSL);
    dlg.setWslSettings(settings_);
    if(dlg.exec() == QDialog::Accepted)
        settings_ = dlg.wslSettings();
}

bool WSLSession::createShell(QMdiArea *midArea, bool isLog)
{
    WSLWidget* widget = new WSLWidget(isLog);
    QMdiSubWindow* subWindow = midArea->addSubWindow(widget);

    subWindow->setWindowTitle(QString("%1.%2").arg(index++).arg(name()));
    subWindow->setWindowIcon(icon());
    subWindow->setSystemMenu(createSystemMenu(subWindow, widget));
    connect(widget, &WSLWidget::onClose, this, &Session::onClose);
    connect(widget, &WSLWidget::onCommand, this, &Session::onCommand);
    connect(widget, &WSLWidget::fontSizeChanged, this, &Session::fontSizeChanged);
    connect(widget, &WSLWidget::highLighterChanged, this, &Session::highLighterChanged);
    connect(widget, &WSLWidget::getHighlighter, this, &WSLSession::setHighLighter);
    connect(subWindow, &QMdiSubWindow::windowStateChanged, this, &Session::windowStateChanged);
    subWindow->setOption(QMdiSubWindow::RubberBandResize);
    subWindow->setOption(QMdiSubWindow::RubberBandMove);
    subWindow->setStyle(QStyleFactory::create("Fusion"));

    ConsoleColor color = ConsoleColorManager::Instance()->color(colorIndex());
    ConsolePaletteManager* manager = ConsolePaletteManager::Instance();
    ConsolePalette::Ptr palette = manager->findPalette(paletteName());

    widget->setCodecName(codecName());
    widget->setFontName(fontName());
    widget->setFontSize(fontSize());
    widget->setConsoleColor(color);
    if(palette)
        widget->setConsolePalette(palette);
    widget->updateHightLighter(hightLighter());
    widget->showNormal();

    widgets_ << widget;
    return true;
}

bool WSLSession::runShell()
{
    if(widgets_.isEmpty())
        return false;
    return widgets_.last()->runShell(settings_);
}

void WSLSession::closeSheel(QWidget *widget)
{
    widgets_.removeAll(dynamic_cast<WSLWidget *>(widget));
}

bool WSLSession::isThisWidget(QWidget *widget)
{
    int index = widgets_.indexOf(dynamic_cast<WSLWidget *>(widget));
    return index != -1;
}

void WSLSession::sendCommand(QWidget *widget, QString const& command)
{
    WSLWidget *theWidget = dynamic_cast<WSLWidget *>(widget);
    if(theWidget)
        theWidget->sendCommand(command);
}

bool WSLSession::isConnected(QWidget *widget)
{
    WSLWidget *theWidget = dynamic_cast<WSLWidget *>(widget);
    if(theWidget)
        return theWidget->isConnected();
    return false;
}

void WSLSession::reconnect(QWidget *widget)
{
    WSLWidget *theWidget = dynamic_cast<WSLWidget *>(widget);
    if(theWidget)
        theWidget->reconnect(settings_);
}

void WSLSession::disconnect(QWidget *widget)
{
    WSLWidget *theWidget = dynamic_cast<WSLWidget *>(widget);
    if(theWidget)
        theWidget->disconnect();
}

bool WSLSession::isDisplay(QWidget *widget) const
{
    WSLWidget *theWidget = dynamic_cast<WSLWidget *>(widget);
    if(theWidget)
        return theWidget->isDisplay();
    return true;
}

void WSLSession::undisplay(QWidget *widget)
{
    WSLWidget *theWidget = dynamic_cast<WSLWidget *>(widget);
    if(theWidget)
        theWidget->undisplay();
}

void WSLSession::display(QWidget *widget)
{
    WSLWidget *theWidget = dynamic_cast<WSLWidget *>(widget);
    if(theWidget)
        theWidget->display();
}

void WSLSession::save(QWidget *widget)
{
    WSLWidget *theWidget = dynamic_cast<WSLWidget *>(widget);
    if(theWidget)
        theWidget->save();
}

void WSLSession::print(QWidget *widget)
{
    WSLWidget *theWidget = dynamic_cast<WSLWidget *>(widget);
    if(theWidget)
        theWidget->print();
}

void WSLSession::copy(QWidget *widget)
{
    WSLWidget *theWidget = dynamic_cast<WSLWidget *>(widget);
    if(theWidget)
        theWidget->copy();
}

void WSLSession::copyAll(QWidget *widget)
{
    WSLWidget *theWidget = dynamic_cast<WSLWidget *>(widget);
    if(theWidget)
        theWidget->copyAll();
}

void WSLSession::paste(QWidget *widget)
{
    WSLWidget *theWidget = dynamic_cast<WSLWidget *>(widget);
    if(theWidget)
        theWidget->paste();
}

void WSLSession::updateHightLighter(QString const& hightLighter)
{
    for(int i = 0; i < widgets_.size(); i++)
        widgets_[i]->updateHightLighter(hightLighter);
}

void WSLSession::updateCodecName(QString const& codecName)
{
    for(int i = 0; i < widgets_.size(); i++)
        widgets_[i]->setCodecName(codecName);
}

void WSLSession::updateFontName(QString const& fontName)
{
    for(int i = 0; i < widgets_.size(); i++)
        widgets_[i]->setFontName(fontName);
}

void WSLSession::updateColorIndex(int index)
{
    ConsoleColor color = ConsoleColorManager::Instance()->color(index);
    for(int i = 0; i < widgets_.size(); i++)
        widgets_[i]->setConsoleColor(color);
}

void WSLSession::updatePaletteName(QString const& paletteName)
{
    ConsolePaletteManager* manager = ConsolePaletteManager::Instance();
    ConsolePalette::Ptr palette = manager->findPalette(paletteName);
    if(palette)
    {
        for(int i = 0; i < widgets_.size(); i++)
            widgets_[i]->setConsolePalette(palette);
    }
}

void WSLSession::updateFontSize(int fontSize)
{
    for(int i = 0; i < widgets_.size(); i++)
        widgets_[i]->setFontSize(fontSize);
}

void WSLSession::increaseFontSize(QWidget *widget)
{
    WSLWidget *theWidget = dynamic_cast<WSLWidget *>(widget);
    if(theWidget)
        theWidget->increaseFontSize();
}

void WSLSession::decreaseFontSize(QWidget *widget)
{
    WSLWidget *theWidget = dynamic_cast<WSLWidget *>(widget);
    if(theWidget)
        theWidget->decreaseFontSize();
}

void WSLSession::clearScrollback(QWidget *widget)
{
    WSLWidget *theWidget = dynamic_cast<WSLWidget *>(widget);
    if(theWidget)
        theWidget->clearScrollback();
}

void WSLSession::setObject(QJsonObject const& obj)
{
    settings_.distributionText = obj.value("distributionText").toString();
    settings_.distribution = obj.value("distribution").toString();
    settings_.startupPath = obj.value("startupPath").toString();
    settings_.specifyUsername = obj.value("specifyUsername").toString();
    settings_.useSpecifyUsername = obj.value("specifyUsername").toBool();
}

QJsonObject WSLSession::object() const
{
    QJsonObject obj;

    obj.insert("distributionText", settings_.distributionText);
    obj.insert("distribution", settings_.distribution);
    obj.insert("startupPath", settings_.startupPath);
    obj.insert("specifyUsername", settings_.specifyUsername);
    obj.insert("useSpecifyUsername", settings_.useSpecifyUsername);
    return obj;
}


void WSLSession::setHighLighter()
{
    for(int i = 0; i < widgets_.size(); i++)
        widgets_[i]->setHighLighter(hightLighter());
}

void WSLSession::updateTitle(QString const& name)
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

QMenu* WSLSession::createSystemMenu(QMdiSubWindow *parent, QWidget *widget)
{
    QMenu* menu = Session::createSystemMenu(parent, widget);
    QList<QAction*> actions = menu->actions();
    QAction* action = new QAction(QIcon(":image/File/copy.png"), tr("Duplicate tab"), menu);
    menu->insertAction(actions.first(), action);
    connect(action, &QAction::triggered, this, [=]{
        emit onCreateShell(widget);
    });
    return menu;
}
