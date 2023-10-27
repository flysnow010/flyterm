#include "serialportwidget.h"
#include "console/serialportconsole.h"
#include "core/serialsettings.h"
#include "transfer/kermit/kermitfilesender.h"
#include "transfer/kermit/kermitfilerecver.h"
#include "transfer/xyzmodem/xymodemfilesender.h"
#include "transfer/xyzmodem/xymodemfilerecver.h"
#include "dialog/fileprogressdialog.h"
#include "util/util.h"

#include <QMenu>
#include <QFileDialog>
#include <QApplication>

SerialPortWidget::SerialPortWidget(bool isLog, QWidget *parent)
    : Child(parent)
    , serial(new QSerialPort())
{
    createConsoleAndConnect();
    connect(serial, SIGNAL(readyRead()), this, SLOT(onData()));
    connect(this, &Child::onClose, this, [=](QWidget *){
        serial->close();
    });
    if(isLog)
    {
        beforeLogfile_ = LogFile::SharedPtr(new LogFile());
        beforeLogfile_->open(QString("%1/serial_%2_%3.txt")
                       .arg(Util::logPath())
                       .arg(uint64_t(this), 8, 16)
                       .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss")));
    }
}

SerialPortWidget::~SerialPortWidget()
{
    delete serial;
}

QString SerialPortWidget::errorString()
{
    QString errorText;
    QSerialPort::SerialPortError error = serial->error();
    if(error == QSerialPort::DeviceNotFoundError)
        errorText = QString(tr("Unable to open serial port %1")).arg(serial->portName());
    else if(error == QSerialPort::OpenError)
        errorText = QString(tr("%1 have be opened")).arg(serial->portName());
    return QString("%1\n%2")
            .arg(errorText, serial->errorString());
}

void SerialPortWidget::setErrorText(QString const& text)
{
    setConsoleText(text);
}

bool SerialPortWidget::isConnected() const
{
    return serial->isOpen();
}

void SerialPortWidget::reconnect()
{
    clearScrollback();
    serial->open(QIODevice::ReadWrite);
}

void SerialPortWidget::disconnect()
{
    serial->close();
}

void SerialPortWidget::recvFileByKermit()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                    filePath, tr("All files (*.*)"));
    if(fileName.isEmpty())
        return;
    filePath = QFileInfo(fileName).filePath();
    recvFileByKermit(fileName);
}

void SerialPortWidget::sendFileByKermit()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    filePath, tr("All files (*.*)"));
    if(fileName.isEmpty())
        return;
    filePath = QFileInfo(fileName).filePath();
    sendFileByKermit(fileName);
}

void SerialPortWidget::recvFileByXModem()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                    filePath, tr("All files (*.*)"));
    if(fileName.isEmpty())
        return;
    filePath = QFileInfo(fileName).filePath();
    recvFileByXYModem(fileName, false);
}

void SerialPortWidget::sendFileByXModem()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    filePath, tr("All files (*.*)"));
    if(fileName.isEmpty())
        return;
    filePath = QFileInfo(fileName).filePath();
    sendFileByXYModem(fileName, false);
}

void SerialPortWidget::recvFileByYModem()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                    filePath, tr("All files (*.*)"));
    if(fileName.isEmpty())
        return;
    filePath = QFileInfo(fileName).filePath();
    recvFileByXYModem(fileName, true);
}

void SerialPortWidget::sendFileByYModem()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    filePath, tr("All files (*.*)"));
    if(fileName.isEmpty())
        return;
    filePath = QFileInfo(fileName).filePath();
    sendFileByXYModem(fileName, true);
}

Console* SerialPortWidget::createConsole()
{
    return new SerialPortConsole(this);
}

void SerialPortWidget::writeToShell(QByteArray const& data)
{
    serial->write(data);
}

void SerialPortWidget::onPullData()
{
    if(datas.isEmpty())
        return;

    QByteArray data;
    while(datas.size() > 0 && data.size() < 512)
        data.push_back(datas.takeFirst());
    onDisplay(data);
    if(beforeLogfile_)
        beforeLogfile_->write(data);
}

void SerialPortWidget::onContextMenu(QMenu &contextMenu)
{
    QMenu* upload = contextMenu.addMenu(tr("Upload"));
    upload->addAction(tr("Kermit Send..."), this, SLOT(sendFileByKermit()));
    upload->addAction(tr("XModem Send..."), this, SLOT(sendFileByXModem()));
    upload->addAction(tr("YModem Send..."), this, SLOT(sendFileByXModem()));
    QMenu* download = contextMenu.addMenu(tr("Download"));
    download->addAction(tr("Kermit Reveive..."), this, SLOT(recvFileByKermit()));
    download->addAction(tr("XModem Reveive..."), this, SLOT(recvFileByXModem()));
    download->addAction(tr("YModem Reveive..."), this, SLOT(recvFileByYModem()));
    contextMenu.addSeparator();
}

void SerialPortWidget::onExpandCommand(QString const& command)
{
    QStringList cmds = command.split(' ');

    if(command.startsWith("#ksend"))
    {
        if(cmds.size() > 1)
            sendFileByKermit(cmds[1]);
    }
    else if(command.startsWith("#xsend"))
    {
        if(cmds.size() > 1)
            sendFileByXYModem(cmds[1], false);
    }
    else if(command.startsWith("#ysend"))
    {
        if(cmds.size() > 1)
            sendFileByXYModem(cmds[1], true);
    }
    else if(command.startsWith("#krecv"))
    {
        if(cmds.size() > 1)
            recvFileByKermit(cmds[1]);
    }
    else if(command.startsWith("#xrecv"))
    {
        if(cmds.size() > 1)
            recvFileByXYModem(cmds[1], false);
    }
    else if(command.startsWith("#yrecv"))
    {
        if(cmds.size() > 1)
            recvFileByXYModem(cmds[1], true);
    }
}

bool SerialPortWidget::runShell(SerialSettings const& settings)
{
    serial->setPortName(settings.port);
    serial->setBaudRate(settings.baudRate);
    serial->setDataBits(settings.dataBits);
    serial->setParity(settings.parity);
    serial->setStopBits(settings.stopBits);
    serial->setFlowControl(settings.flowControl);
    console()->setLocalEchoEnabled(settings.localEchoEnabled);
    bool isOK = serial->open(QIODevice::ReadWrite);
    display();
    return isOK;
}

void SerialPortWidget::onExecCommand(QString const& command)
{
    serial->write(QString("%1\r").arg(command).toUtf8());
}

void SerialPortWidget::onData()
{
    QByteArray data = serial->readAll();
    datas.push_back(data);
}

void SerialPortWidget::sendFileByKermit(QString const& fileName)
{
    QObject::disconnect(serial, SIGNAL(readyRead()), this, SLOT(onData()));

    FileProgressDialog dialog(this);
    KermitFileSender sender(serial);

    connect(&sender, &KermitFileSender::gotFileSize, &dialog, &FileProgressDialog::setFileSize);
    connect(&sender, &KermitFileSender::progressInfo, &dialog, &FileProgressDialog::setProgressInfo);
    connect(&sender, &KermitFileSender::finished, &dialog, &FileProgressDialog::finished);
    connect(&sender, &KermitFileSender::error, &dialog, &FileProgressDialog::error);

    dialog.setTitle(tr("Kermit Send"));
    dialog.setProtocol("Kermit");
    dialog.setFilename(QFileInfo(fileName).fileName());
    dialog.setModal(true);
    dialog.setVisible(true);

    sender.start(fileName);
    while(!dialog.isFinished())
    {
        if(dialog.isCancel())
        {
            sender.stop();
            while(!dialog.isFinished())
                QApplication::processEvents();
            sender.cancel();
        }
        QApplication::processEvents();
    }

    connect(serial, SIGNAL(readyRead()), this, SLOT(onData()));
}

void SerialPortWidget::recvFileByKermit(QString const& fileName)
{
    QObject::disconnect(serial, SIGNAL(readyRead()), this, SLOT(onData()));
    FileProgressDialog dialog(this);
    KermitFileRecver recver(serial);
    connect(&recver, &KermitFileRecver::gotFileSize, &dialog, &FileProgressDialog::setFileSize);
    connect(&recver, &KermitFileRecver::progressInfo, &dialog, &FileProgressDialog::setProgressInfo);
    connect(&recver, &KermitFileRecver::finished, &dialog, &FileProgressDialog::finished);
    connect(&recver, &KermitFileRecver::error, &dialog, &FileProgressDialog::error);

    dialog.setTitle(tr("Kermit Recv"));
    dialog.setProtocol("Kermit");

    dialog.setFilename(QFileInfo(fileName).fileName());
    dialog.setModal(true);
    dialog.setVisible(true);

    recver.start(fileName);
    while(!dialog.isFinished())
    {
        if(dialog.isCancel())
        {
            recver.stop();
            while(!dialog.isFinished())
                QApplication::processEvents();
            recver.cancel();
        }
        QApplication::processEvents();
    }

    connect(serial, SIGNAL(readyRead()), this, SLOT(onData()));
}

void SerialPortWidget::sendFileByXYModem(QString const& fileName, bool isYModem)
{
    QObject::disconnect(serial, SIGNAL(readyRead()), this, SLOT(onData()));

    FileProgressDialog dialog(this);
    XYModemFileSender sender(serial, isYModem);

    connect(&sender, &XYModemFileSender::gotFileSize, &dialog, &FileProgressDialog::setFileSize);
    connect(&sender, &XYModemFileSender::progressInfo, &dialog, &FileProgressDialog::setProgressInfo);
    connect(&sender, &XYModemFileSender::finished, &dialog, &FileProgressDialog::finished);
    connect(&sender, &XYModemFileSender::error, &dialog, &FileProgressDialog::error);

    if(isYModem)
    {
        dialog.setTitle(tr("YMODEM Send"));
        dialog.setProtocol(tr("1K YMODEM"));
    }
    else
    {
        dialog.setTitle(tr("XMODEM Send"));
        dialog.setProtocol(tr("1K XMODEM"));
    }
    dialog.setFilename(QFileInfo(fileName).fileName());
    dialog.setModal(true);
    dialog.setVisible(true);

    sender.start(fileName);
    while(!dialog.isFinished())
    {
        if(dialog.isCancel())
        {
            sender.stop();
            while(!dialog.isFinished())
                QApplication::processEvents();
            sender.cancel();
        }
        QApplication::processEvents();
    }

    connect(serial, SIGNAL(readyRead()), this, SLOT(onData()));
}

void SerialPortWidget::recvFileByXYModem(QString const& fileName, bool isYModem)
{
    QObject::disconnect(serial, SIGNAL(readyRead()), this, SLOT(onData()));
    FileProgressDialog dialog(this);
    XYModemFileRecver recver(serial, isYModem);
    connect(&recver, &XYModemFileRecver::gotFileSize, &dialog, &FileProgressDialog::setFileSize);
    connect(&recver, &XYModemFileRecver::progressInfo, &dialog, &FileProgressDialog::setProgressInfo);
    connect(&recver, &XYModemFileRecver::finished, &dialog, &FileProgressDialog::finished);
    connect(&recver, &XYModemFileRecver::error, &dialog, &FileProgressDialog::error);

    if(isYModem)
    {
        dialog.setTitle(tr("YMODEM Recv"));
        dialog.setProtocol(tr("1K YMODEM"));
    }
    else
    {
        dialog.setTitle(tr("XMODEM Recv"));
        dialog.setProtocol(tr("1K XMODEM"));
    }

    dialog.setFilename(QFileInfo(fileName).fileName());
    dialog.setModal(true);
    dialog.setVisible(true);

    recver.start(fileName);
    while(!dialog.isFinished())
    {
        if(dialog.isCancel())
        {
            recver.stop();
            while(!dialog.isFinished())
                QApplication::processEvents();
            recver.cancel();
        }
        QApplication::processEvents();
    }

    connect(serial, SIGNAL(readyRead()), this, SLOT(onData()));
}
