#include "serialsession.h"
#include "child/serialportwidget.h"
#include "dialog/connectdialog.h"
#include "color/consolecolor.h"
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QStyleFactory>

SerialSession::SerialSession(QString const& name)
    : Session(name)
{

}

SerialSession::SerialSession(QString const& name, SerialSettings const& settings)
    : Session(name)
    , settings_(settings)
{

}

QString SerialSession::type() const
{
    return QString("SerialSession");
}

QIcon SerialSession::icon()
{
    return QIcon(":image/Term/serial.png");
}

void SerialSession::edit()
{
    ConnectDialog dlg;
    dlg.setType(ConnectType::Serial);
    dlg.setSerialSettings(settings_);
    if(dlg.exec() == QDialog::Accepted)
        settings_ = dlg.serialSettings();
}

bool SerialSession::createShell(QMdiArea *midArea, bool isLog)
{
    if(widget_)
        return false;

    widget_ = new SerialPortWidget(isLog);
    QMdiSubWindow* subWindow = midArea->addSubWindow(widget_);
    subWindow->setWindowTitle(name());
    subWindow->setWindowIcon(icon());

    connect(widget_, &SerialPortWidget::onClose, this, &Session::onClose);
    connect(widget_, &SerialPortWidget::onCommand, this, &Session::onCommand);
    connect(widget_, &SerialPortWidget::fontSizeChanged, this, &Session::fontSizeChanged);
    connect(widget_, &SerialPortWidget::highLighterChanged, this, &Session::highLighterChanged);
    connect(widget_, &SerialPortWidget::getHighlighter, this, &SerialSession::setHighLighter);
    connect(subWindow, &QMdiSubWindow::windowStateChanged, this, &Session::windowStateChanged);
    subWindow->setOption(QMdiSubWindow::RubberBandResize);
    subWindow->setOption(QMdiSubWindow::RubberBandMove);
    subWindow->setStyle(QStyleFactory::create("Fusion"));

    ConsoleColor color = ConsoleColorManager::Instance()->color(colorIndex());
    ConsolePaletteManager* manager = ConsolePaletteManager::Instance();
    ConsolePalette::Ptr palette = manager->findPalette(paletteName());

    widget_->setCodecName(codecName());
    widget_->setFontName(fontName());
    widget_->setFontSize(fontSize());
    widget_->setConsoleColor(color);
    if(palette)
        widget_->setConsolePalette(palette);
    widget_->updateHightLighter(hightLighter());
    widget_->showMaximized();
    return true;
}

void SerialSession::setHighLighter()
{
    if(widget_)
        widget_->setHighLighter(hightLighter());
}

void SerialSession::updateTitle(QString const& name)
{
    if(widget_)
    {
        QWidget* subWindow = widget_->parentWidget();
        if(subWindow)
            subWindow->setWindowTitle(name);
    }
}

bool SerialSession::runShell()
{
    bool isOK = false;
    if(widget_)
    {
        isOK = widget_->runShell(settings_);
        if(!isOK)
            widget_->setErrorText(widget_->errorString());
    }
    return isOK;
}

void SerialSession::closeSheel(QWidget *widget)
{
    if(widget == widget_)
        widget_ = 0;
}

void SerialSession::sendCommand(QWidget *widget, QString const& command)
{
    SerialPortWidget *theWidget = dynamic_cast<SerialPortWidget *>(widget);
    if(theWidget)
        theWidget->sendCommand(command);
}

void SerialSession::disconnect(QWidget *widget)
{
    SerialPortWidget *theWidget = dynamic_cast<SerialPortWidget *>(widget);
    if(theWidget)
        theWidget->disconnect();
}

void SerialSession::save(QWidget *widget)
{
    SerialPortWidget *theWidget = dynamic_cast<SerialPortWidget *>(widget);
    if(theWidget)
        theWidget->save();
}

void SerialSession::print(QWidget *widget)
{
    SerialPortWidget *theWidget = dynamic_cast<SerialPortWidget *>(widget);
    if(theWidget)
        theWidget->print();
}

void SerialSession::copy(QWidget *widget)
{
    SerialPortWidget *theWidget = dynamic_cast<SerialPortWidget *>(widget);
    if(theWidget)
        theWidget->copy();
}

void SerialSession::copyAll(QWidget *widget)
{
    SerialPortWidget *theWidget = dynamic_cast<SerialPortWidget *>(widget);
    if(theWidget)
        theWidget->copyAll();
}

void SerialSession::paste(QWidget *widget)
{
    SerialPortWidget *theWidget = dynamic_cast<SerialPortWidget *>(widget);
    if(theWidget)
        theWidget->paste();
}

void SerialSession::updateCodecName(QString const& codecName)
{
    if(widget_)
        widget_->setCodecName(codecName);
}

void SerialSession::updateFontName(QString const& name)
{
    if(widget_)
        widget_->setFontName(name);
}

void SerialSession::updateColorIndex(int index)
{
    ConsoleColor color = ConsoleColorManager::Instance()->color(index);
    if(widget_)
        widget_->setConsoleColor(color);
}

void SerialSession::updatePaletteName(QString const&)
{
}

void SerialSession::updateHightLighter(QString const& hightLighter)
{
    if(widget_)
        widget_->updateHightLighter(hightLighter);
}

void SerialSession::updateFontSize(int fontSize)
{
    if(widget_)
        widget_->setFontSize(fontSize);
}

void SerialSession::increaseFontSize(QWidget *widget)
{
    SerialPortWidget *theWidget = dynamic_cast<SerialPortWidget *>(widget);
    if(theWidget)
        theWidget->increaseFontSize();
}

void SerialSession::decreaseFontSize(QWidget *widget)
{
    SerialPortWidget *theWidget = dynamic_cast<SerialPortWidget *>(widget);
    if(theWidget)
        theWidget->decreaseFontSize();
}

void SerialSession::clearScrollback(QWidget *widget)
{
    SerialPortWidget *theWidget = dynamic_cast<SerialPortWidget *>(widget);
    if(theWidget)
        theWidget->clearScrollback();
}

void SerialSession::sendFile(QWidget *widget, QString const& protocol)
{
    SerialPortWidget *theWidget = dynamic_cast<SerialPortWidget *>(widget);
    if(theWidget)
    {
        if(protocol == "Kermit")
            theWidget->sendFileByKermit();
        else if(protocol == "XMODEM")
            theWidget->sendFileByXModem();
        else if(protocol == "YMODEM")
            theWidget->sendFileByYModem();
    }
}

void SerialSession::recvFile(QWidget *widget, QString const& protocol)
{
    SerialPortWidget *theWidget = dynamic_cast<SerialPortWidget *>(widget);
    if(theWidget)
    {
        if(protocol == "Kermit")
            theWidget->recvFileByKermit();
        else if(protocol == "XMODEM")
            theWidget->recvFileByXModem();
        else if(protocol == "YMODEM")
            theWidget->recvFileByYModem();
    }
}

bool SerialSession::isThisWidget(QWidget *widget)
{
    return widget == widget_;
}

int  SerialSession::shellSize()
{
    return widget_ != nullptr ? 1 : 0;
}

void SerialSession::setObject(QJsonObject const& obj)
{
    settings_.port = obj.value("port").toString();
    settings_.baudRate = obj.value("baudRate").toInt();
    settings_.dataBits = static_cast<QSerialPort::DataBits>(obj.value("dataBits").toInt());
    settings_.parity = static_cast<QSerialPort::Parity>(obj.value("parity").toInt());
    settings_.stopBits = static_cast<QSerialPort::StopBits>(obj.value("stopBits").toInt());
    settings_.flowControl = static_cast<QSerialPort::FlowControl>(obj.value("flowControl").toInt());
    settings_.localEchoEnabled = obj.value("localEchoEnabled").toBool();
}

QJsonObject SerialSession::object() const
{
    QJsonObject obj;
    obj.insert("port", settings_.port);
    obj.insert("baudRate", settings_.baudRate);
    obj.insert("dataBits", settings_.dataBits);
    obj.insert("parity", settings_.parity);
    obj.insert("stopBits", settings_.stopBits);
    obj.insert("flowControl", settings_.flowControl);
    obj.insert("localEchoEnabled", settings_.localEchoEnabled);
    return  obj;
}
