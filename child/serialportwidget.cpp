#include "serialportwidget.h"
#include "console/serialportconsole.h"
#include "core/commandthread.h"
#include "transfer/kermit/kermitfilesender.h"
#include "transfer/kermit/kermitfilerecver.h"
#include "transfer/xyzmodem/xymodemfilesender.h"
#include "transfer/xyzmodem/xymodemfilerecver.h"
#include "dialog/fileprogressdialog.h"
#include "highlighter/hightlightermanager.h"
#include "util/util.h"

#include <QPrinter>
#include <QPrintDialog>
#include <QMenu>
#include <QResizeEvent>
#include <QFileDialog>
#include <QTimer>
#include <QtDebug>
#include <QApplication>

SerialPortWidget::SerialPortWidget(bool isLog, QWidget *parent)
    : QWidget(parent)
    , console(new SerialPortConsole(this))
    , commandThread_(new CommandThread(this))
    , serial(new QSerialPort())
    , dataTimer(new QTimer(this))
{
    setAttribute(Qt::WA_DeleteOnClose);
    console->createParserAndConnect();
    if(isLog)
    {
        beforeLogfile_ = LogFile::SharedPtr(new LogFile());
        beforeLogfile_->open(QString("%1/serial_%2_%3.txt")
                       .arg(Util::logoPath())
                       .arg(uint64_t(this), 8, 16)
                       .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss")));
    }
    connect(dataTimer, SIGNAL(timeout()), this, SLOT(pullData()));
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(commandThread_, SIGNAL(onAllCommand(QString)), this, SIGNAL(onCommand(QString)));
    connect(commandThread_, SIGNAL(onCommand(QString)), this, SLOT(execCommand(QString)));
    connect(commandThread_, SIGNAL(onExpandCommand(QString)),
            this, SLOT(execExpandCommand(QString)), Qt::BlockingQueuedConnection);
    connect(commandThread_, SIGNAL(onTestCommand(QString)),
            this, SLOT(execTestCommand(QString)));
    connect(console, SIGNAL(onGotCursorPos(int,int)), this, SLOT(onGotCursorPos(int,int)));
    connect(console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));
    connect(console, &QWidget::customContextMenuRequested,
            this, &SerialPortWidget::customContextMenu);
    commandThread_->start();
    dataTimer->start(1);
}

SerialPortWidget::~SerialPortWidget()
{
    delete serial;
}

void SerialPortWidget::resizeEvent(QResizeEvent *event)
{
    console->resize(event->size());
}

void SerialPortWidget::closeEvent(QCloseEvent *event)
{
    emit onClose(this);
    event->accept();
    commandThread_->stop();
    commandThread_->wait();
    commandThread_->quit();
}

QSize SerialPortWidget::sizeHint() const
{
    return QSize(400, 300);
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
    console->setPlainText(text);
}

bool SerialPortWidget::isConnected() const
{
    return serial->isOpen();
}

void SerialPortWidget::reconnect()
{
    console->clearall();
    serial->open(QIODevice::ReadWrite);
}

void SerialPortWidget::disconnect()
{
    serial->close();
}

bool SerialPortWidget::isDisplay() const
{
    return dataTimer->isActive();
}

void SerialPortWidget::display()
{
    dataTimer->start(1);
}

void SerialPortWidget::undisplay()
{
    dataTimer->stop();
}

void SerialPortWidget::save()
{
    console->saveToFile();
}

void SerialPortWidget::print()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (console->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted)
        console->print(&printer);
    delete dlg;
}

void SerialPortWidget::updateHightLighter(QString const& hightLighter)
{
    console->updateHightLighter(hightLighter);
}

void SerialPortWidget::setHighLighter(QString const& hightLighter)
{
    highLight_ = hightLighter;
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

void SerialPortWidget::copy()
{
    console->copyOne();
}

void SerialPortWidget::copyAll()
{
    console->copyAll();
}

void SerialPortWidget::paste()
{
    console->paste();
}

void SerialPortWidget::setCodecName(QString const& name)
{
    console->setCodecName(name);
}

void SerialPortWidget::setFontName(QString const& name)
{
    console->setFontName(name);
}

void SerialPortWidget::setConsoleColor(ConsoleColor const& color)
{
   console->setConsoleColor(color);
}

void SerialPortWidget::setFontSize(int fontSize)
{
    console->setFontSize(fontSize);
}

void SerialPortWidget::setConsolePalette(ConsolePalette::Ptr palette)
{
    console->setConsolePalette(palette);
}

void SerialPortWidget::increaseFontSize()
{
    if(console->increaseFontSize())
        emit fontSizeChanged(console->fontSize());
}
void SerialPortWidget::decreaseFontSize()
{
    if(console->decreaseFontSize())
        emit fontSizeChanged(console->fontSize());
}

void SerialPortWidget::clearScrollback()
{
    console->clearall();
}

void SerialPortWidget::pullData()
{
    if(datas.isEmpty())
        return;

    QByteArray data;
    while(datas.size() > 0 && data.size() < 512)
        data.push_back(datas.takeFirst());

    if(isTest_)
    {
        testData_.push_back(data);
        if(testData_.contains(testParam_))
        {
            QString command = getTestCommand();
            if(!command.startsWith("#"))
                execCommand(command);
            else
            {
                execExpandCommand(command);
                execCommand(QString());
            }
            testData_.clear();
            if(testCommandIsEmpty())
                isTest_ = false;
        }
    }
    if(beforeLogfile_)
        beforeLogfile_->write(data);
    console->putData(data);
}

bool SerialPortWidget::runShell(QString const& name, int baudRate)
{
    serial->setPortName(name);
    serial->setBaudRate(baudRate);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    return serial->open(QIODevice::ReadWrite);
}

bool SerialPortWidget::runShell(SerialSettings const& settings)
{
    serial->setPortName(settings.port);
    serial->setBaudRate(settings.baudRate);
    serial->setDataBits(settings.dataBits);
    serial->setParity(settings.parity);
    serial->setStopBits(settings.stopBits);
    serial->setFlowControl(settings.flowControl);
    console->setLocalEchoEnabled(settings.localEchoEnabled);
    return serial->open(QIODevice::ReadWrite);
}

void SerialPortWidget::sendCommand(QString const& command)
{
    QStringList commands = command.split("\n");
    sendCommands(commands);
}

void SerialPortWidget::sendCommands(QStringList const& commands)
{
    for(int i = 0; i < commands.size(); i++)
    {
        QString command = commands.at(i);
        if(command.isEmpty())
            continue;
        commandThread_->postCommand(command);
    }
}

void SerialPortWidget::execCommand(QString const& command)
{
    serial->write(QString("%1\r").arg(command).toUtf8());
}

void SerialPortWidget::execTestCommand(QString const& command)
{
    QString testCommand = command.right(command.size() - 1);
    if(!testCommand.startsWith("start"))
        testCommands_.push_back(testCommand);
    else
    {
        QStringList cmds = testCommand.split(' ');
        if(cmds.size() > 1)
            testParam_ = cmds[1].toUtf8();
        isTest_ = true;
        QString cmd = getTestCommand();
        if(!cmd.startsWith("#"))
            execCommand(cmd);
        else
        {
            execExpandCommand(cmd);
            execCommand(QString());
        }
    }
}

void SerialPortWidget::execExpandCommand(QString const& command)
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
    else if(command.startsWith("#bsave"))
    {
        if(cmds.size() > 1)
        {
            LogFile::SharedPtr logfile(new LogFile());
            if(logfile->open(cmds[1], false))
            {
                afterLogfile_ = logfile;
                console->setLogFile(afterLogfile_);
            }
        }
    }
    else if(command.startsWith("#esave"))
    {
        afterLogfile_ = LogFile::SharedPtr();
    }
}

void SerialPortWidget::readData()
{
    QByteArray data = serial->readAll();
    datas.push_back(data);
}

void SerialPortWidget::writeData(QByteArray const&data)
{
    serial->write(data);
}

void SerialPortWidget::customContextMenu(const QPoint &)
{
    QMenu contextMenu;

    contextMenu.addAction(tr("Copy"), this, SLOT(copy()));
    contextMenu.addAction(tr("Copy All"), this, SLOT(copyAll()));
    contextMenu.addAction(tr("Paste"), this, SLOT(paste()));
    contextMenu.addSeparator();
    contextMenu.addAction(tr("Increase Font Size"), this, SLOT(increaseFontSize()));
    contextMenu.addAction(tr("Decrease Font Size"), this, SLOT(decreaseFontSize()));
    contextMenu.addSeparator();
    QMenu* upload = contextMenu.addMenu(tr("Upload"));
    upload->addAction(tr("Kermit Send..."), this, SLOT(sendFileByKermit()));
    upload->addAction(tr("XModem Send..."), this, SLOT(sendFileByXModem()));
    upload->addAction(tr("YModem Send..."), this, SLOT(sendFileByXModem()));
    QMenu* download = contextMenu.addMenu(tr("Download"));
    download->addAction(tr("Kermit Reveive..."), this, SLOT(recvFileByKermit()));
    download->addAction(tr("XModem Reveive..."), this, SLOT(recvFileByXModem()));
    download->addAction(tr("YModem Reveive..."), this, SLOT(recvFileByYModem()));

    contextMenu.addSeparator();
    createHighLightMenu(contextMenu.addMenu(tr("Syntax Highlighting")));
    contextMenu.addSeparator();
    contextMenu.addAction(tr("Save To File..."), this, SLOT(save()));
    contextMenu.addAction(tr("Clear Scrollback"), this, SLOT(clearScrollback()));

    contextMenu.exec(QCursor::pos());
    console->cancelSelection();
}

void SerialPortWidget::createHighLightMenu(QMenu* menu)
{
    emit getHighlighter();
    for(int i = 0; i < highLighterManager->size(); i++)
    {
        HighLighterManager::HighLighter const& lighter = highLighterManager->highLighter(i);
        QAction *action = menu->addAction(lighter.text, this, SLOT(setHighLighter()));
        action->setData(lighter.name);
        action->setCheckable(true);
        if(highLight_ == lighter.name)
            action->setChecked(true);
    }
}

void SerialPortWidget::setHighLighter()
{
    QAction* actoin = (QAction*)sender();
    if(actoin)
        emit highLighterChanged(actoin->data().toString());
}

void SerialPortWidget::onGotCursorPos(int row, int col)
{
    QString cursorPos = QString("\033[%1;%2R").arg(row).arg(col);
    serial->write(cursorPos.toUtf8());
}

void SerialPortWidget::sendFileByKermit(QString const& fileName)
{
    QObject::disconnect(serial, SIGNAL(readyRead()), this, SLOT(readData()));

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

    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
}

void SerialPortWidget::recvFileByKermit(QString const& fileName)
{
    QObject::disconnect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
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

    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
}

void SerialPortWidget::sendFileByXYModem(QString const& fileName, bool isYModem)
{
    QObject::disconnect(serial, SIGNAL(readyRead()), this, SLOT(readData()));

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

    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
}

void SerialPortWidget::recvFileByXYModem(QString const& fileName, bool isYModem)
{
    QObject::disconnect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
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

    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
}

QString SerialPortWidget::getTestCommand()
{
    if(testCommands_.isEmpty())
        return QString();
    QString command = testCommands_.front();
    testCommands_.pop_front();
    return command;
}

bool SerialPortWidget::testCommandIsEmpty() const
{
    return testCommands_.isEmpty();
}
