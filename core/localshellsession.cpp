#include "localshellsession.h"
#include "child/localshellwidget.h"
#include "dialog/connectdialog.h"
#include "util/util.h"

#include <QMdiArea>
#include <QMenu>
#include <QMdiSubWindow>
#include <QApplication>
#include <QStyleFactory>

LocalShellSession::LocalShellSession(QString const& name)
    : Session(name)
{
}

LocalShellSession::LocalShellSession(QString const& name,
                                     LocalShellSettings const& settings)
    : Session(name)
    , settings_(settings)
{
}

QString LocalShellSession::type() const
{
    if(settings_.shellType == "cmd")
        return QString("CmdSession");
    else if(settings_.shellType == "powershell")
        return QString("PowerShellSession");
    return QString();
}

QIcon LocalShellSession::icon()
{
    if(settings_.shellType == "cmd")
        return Util::GetIcon("cmd.exe");
    else
        return Util::GetIcon("powershell.exe");
}

void LocalShellSession::edit()
{
    ConnectDialog dlg;
    dlg.setType(ConnectType::Local);
    dlg.setLocalShellSettings(settings_);
    if(dlg.exec() == QDialog::Accepted)
        settings_ = dlg.localShellSettings();
}

bool LocalShellSession::createShell(QMdiArea *midArea, bool isLog)
{
    LocalShellWidget* widget = new LocalShellWidget(isLog);
    QMdiSubWindow* subWindow = midArea->addSubWindow(widget);

    subWindow->setWindowTitle(QString("%1.%2").arg(index++).arg(name()));
    subWindow->setWindowIcon(icon());
    subWindow->setSystemMenu(createSystemMenu(subWindow, widget));
    connect(widget, &LocalShellWidget::onClose, this, &Session::onClose);
    connect(widget, &LocalShellWidget::onCommand, this, &Session::onCommand);
    connect(widget, &LocalShellWidget::fontSizeChanged, this, &Session::fontSizeChanged);
    connect(widget, &LocalShellWidget::highLighterChanged, this, &Session::highLighterChanged);
    connect(widget, &LocalShellWidget::getHighlighter, this, &LocalShellSession::setHighLighter);
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

void LocalShellSession::setHighLighter()
{
    for(int i = 0; i < widgets_.size(); i++)
        widgets_[i]->setHighLighter(hightLighter());
}

void LocalShellSession::updateTitle(QString const& name)
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

QMenu* LocalShellSession::createSystemMenu(QMdiSubWindow *parent, QWidget *widget)
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

bool LocalShellSession::runShell()
{
    if(widgets_.isEmpty())
        return false;
    return widgets_.last()->runShell(settings_);
}

void LocalShellSession::sendCommand(QWidget *widget, QString const& command)
{
    LocalShellWidget *theWidget = dynamic_cast<LocalShellWidget *>(widget);
    if(theWidget)
        theWidget->sendCommand(command);
}

void LocalShellSession::closeSheel(QWidget *widget)
{
    widgets_.removeAll(dynamic_cast<LocalShellWidget *>(widget));
}

bool LocalShellSession::isConnected(QWidget *widget)
{
    LocalShellWidget *theWidget = dynamic_cast<LocalShellWidget *>(widget);
    if(theWidget)
        return theWidget->isConnected();
    return false;
}

void LocalShellSession::reconnect(QWidget *widget)
{
    LocalShellWidget *theWidget = dynamic_cast<LocalShellWidget *>(widget);
    if(theWidget)
        theWidget->reconnect(settings_);
}

void LocalShellSession::disconnect(QWidget *widget)
{
    LocalShellWidget *theWidget = dynamic_cast<LocalShellWidget *>(widget);
    if(theWidget)
        theWidget->disconnect();
}

bool LocalShellSession::isDisplay(QWidget *widget) const
{
    LocalShellWidget *theWidget = dynamic_cast<LocalShellWidget *>(widget);
    if(theWidget)
        return theWidget->isDisplay();
    return true;
}


void LocalShellSession::display(QWidget *widget)
{
    LocalShellWidget *theWidget = dynamic_cast<LocalShellWidget *>(widget);
    if(theWidget)
        theWidget->display();
}

void LocalShellSession::undisplay(QWidget *widget)
{
    LocalShellWidget *theWidget = dynamic_cast<LocalShellWidget *>(widget);
    if(theWidget)
        theWidget->undisplay();
}

void LocalShellSession::save(QWidget *widget)
{
    LocalShellWidget *theWidget = dynamic_cast<LocalShellWidget *>(widget);
    if(theWidget)
        theWidget->save();
}

void LocalShellSession::print(QWidget *widget)
{
    LocalShellWidget *theWidget = dynamic_cast<LocalShellWidget *>(widget);
    if(theWidget)
        theWidget->print();
}

void LocalShellSession::copy(QWidget *widget)
{
    LocalShellWidget *theWidget = dynamic_cast<LocalShellWidget *>(widget);
    if(theWidget)
        theWidget->copy();
}

void LocalShellSession::copyAll(QWidget *widget)
{
    LocalShellWidget *theWidget = dynamic_cast<LocalShellWidget *>(widget);
    if(theWidget)
        theWidget->copyAll();
}

void LocalShellSession::paste(QWidget *widget)
{
    LocalShellWidget *theWidget = dynamic_cast<LocalShellWidget *>(widget);
    if(theWidget)
        theWidget->paste();
}

void LocalShellSession::updateHightLighter(QString const& hightLighter)
{
    for(int i = 0; i < widgets_.size(); i++)
        widgets_[i]->updateHightLighter(hightLighter);
}

void LocalShellSession::updateCodecName(QString const& codecName)
{
    for(int i = 0; i < widgets_.size(); i++)
        widgets_[i]->setCodecName(codecName);
}

void LocalShellSession::updateFontName(QString const& fontName)
{
    for(int i = 0; i < widgets_.size(); i++)
        widgets_[i]->setFontName(fontName);
}

void LocalShellSession::updateColorIndex(int index)
{
    ConsoleColor color = ConsoleColorManager::Instance()->color(index);
    for(int i = 0; i < widgets_.size(); i++)
        widgets_[i]->setConsoleColor(color);
}

void LocalShellSession::updatePaletteName(QString const& paletteName)
{
    ConsolePaletteManager* manager = ConsolePaletteManager::Instance();
    ConsolePalette::Ptr palette = manager->findPalette(paletteName);
    if(palette)
    {
        for(int i = 0; i < widgets_.size(); i++)
            widgets_[i]->setConsolePalette(palette);
    }
}

void LocalShellSession::updateFontSize(int fontSize)
{
    for(int i = 0; i < widgets_.size(); i++)
        widgets_[i]->setFontSize(fontSize);
}

void LocalShellSession::increaseFontSize(QWidget *widget)
{
    LocalShellWidget *theWidget = dynamic_cast<LocalShellWidget *>(widget);
    if(theWidget)
        theWidget->increaseFontSize();
}

void LocalShellSession::decreaseFontSize(QWidget *widget)
{
    LocalShellWidget *theWidget = dynamic_cast<LocalShellWidget *>(widget);
    if(theWidget)
        theWidget->decreaseFontSize();
}

void LocalShellSession::clearScrollback(QWidget *widget)
{
    LocalShellWidget *theWidget = dynamic_cast<LocalShellWidget *>(widget);
    if(theWidget)
        theWidget->clearScrollback();
}

bool LocalShellSession::isThisWidget(QWidget *widget)
{
    int index = widgets_.indexOf(dynamic_cast<LocalShellWidget *>(widget));
    return index != -1;
}

int  LocalShellSession::shellSize()
{
    return widgets_.size();
}

void LocalShellSession::setObject(QJsonObject const& obj)
{
    settings_.shellText = obj.value("shellText").toString();
    settings_.shellType = obj.value("shellType").toString();
    settings_.startupPath = obj.value("startupPath").toString();
    settings_.executeCommand = obj.value("executeCommand").toString();
}

QJsonObject LocalShellSession::object() const
{
    QJsonObject obj;

    obj.insert("shellText", settings_.shellText);
    obj.insert("shellType", settings_.shellType);
    obj.insert("startupPath", settings_.startupPath);
    obj.insert("executeCommand", settings_.executeCommand);
    return obj;
}

