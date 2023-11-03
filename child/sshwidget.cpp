#include "sshwidget.h"
#include "console/sshconsole.h"
#include "console/alternateconsole.h"
#include "core/sshsettings.h"
#include "core/sshparser.h"
#include "core/commandthread.h"
#include "core/userauth.h"
#include "dialog/passworddialog.h"
#include "dialog/fileprogressdialog.h"
#include "highlighter/hightlightermanager.h"
#include "transfer/sftp/sftptransfer.h"
#include "util/util.h"

#include <QPrinter>
#include <QMenu>
#include <QPrintDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QTimer>
#include <QDebug>
#include <QThread>
#include <QApplication>

SShWidget::SShWidget(bool isLog, QWidget *parent)
    : QWidget(parent)
    , console(new SshConsole(this))
    , alternateConsole(new AlternateConsole(this))
    , commandThread_(new CommandThread(this))
    , commandParser(new SShParser())
    , shell(new SshShell(this))
    , dataTimer(new QTimer(this))
{
    setAttribute(Qt::WA_DeleteOnClose);
    alternateConsole->hide();
    console->setCommandParser(commandParser);
    console->connectCommand();
    alternateConsole->setCommandParser(commandParser);
    if(isLog)
    {
        beforeLogfile_ = LogFile::SharedPtr(new LogFile());
        beforeLogfile_->open(QString("%1/ssh_%2_%3.txt")
                       .arg(Util::logPath())
                       .arg(uint64_t(this), 8, 16)
                       .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss")));
    }
    connect(dataTimer, SIGNAL(timeout()), this, SLOT(pullData()));
    connect(commandThread_, SIGNAL(onAllCommand(QString)), this, SIGNAL(onCommand(QString)));
    connect(commandThread_, SIGNAL(onCommand(QString)), this, SLOT(execCommand(QString)));
    connect(commandThread_, SIGNAL(onExpandCommand(QString)),
            this, SLOT(execExpandCommand(QString)), Qt::BlockingQueuedConnection);
    connect(commandThread_, SIGNAL(onTestCommand(QString)),
            this, SLOT(execTestCommand(QString)));

    connect(shell, SIGNAL(connected()), this, SLOT(connected()));
    connect(shell, SIGNAL(connectionError(QString)), this, SLOT(connectionError(QString)));
    connect(shell, SIGNAL(onError(QByteArray)), this, SLOT(onError(QByteArray)));

    connect(console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));
    connect(console, SIGNAL(onGotCursorPos(int,int)), this, SLOT(onGotCursorPos(int,int)));
    connect(console, SIGNAL(onSwitchToAlternateScreen()), this, SLOT(switchToAlternateScreen()));
    connect(console, SIGNAL(onSwitchToAlternateFinished()), alternateConsole, SLOT(updateRows()));
    connect(console, SIGNAL(onSwitchToAppKeypadMode()), this, SLOT(switchToAppKeypadMode()));
    connect(console, SIGNAL(onTitle(QString)), this, SIGNAL(onTitle(QString)));
    connect(console, &QWidget::customContextMenuRequested, this, &SShWidget::customContextMenu);

    connect(alternateConsole, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));
    connect(alternateConsole, SIGNAL(onSwitchToMainScreen()), this, SLOT(switchToMainScreen()));
    connect(alternateConsole, SIGNAL(onSwitchToNormalKeypadMode()), this, SLOT(switchToNormalKeypadMode()));
    connect(alternateConsole, SIGNAL(onSecondaryDA()), this, SLOT(secondaryDA()));
    connect(alternateConsole, &QWidget::customContextMenuRequested, this, &SShWidget::customContextMenu);

    commandThread_->start();
    dataTimer->start(1);
}

SShWidget::~SShWidget()
{
    delete commandParser;
}

bool SShWidget::runShell(SSHSettings const& settings)
{
    if(settings.usePrivateKey)
        shell->connectTo(settings);
    else
    {
        SSHSettings newSettings = settings;
        QString key = UserAuth::hash(settings.key());
        UserAuth::Ptr userAuth = UserAuthManager::Instance()->findUserAuth(key);

        bool isSavePassword  = false;
        if(userAuth)
            newSettings.passWord = userAuth->auth;
        else
        {
            PasswordDialog dialog;
            dialog.setPromptText(QString(tr("Password for %1"))
                                 .arg(newSettings.name()));
            if(dialog.exec() == QDialog::Accepted)
            {
                newSettings.passWord = dialog.password();
                if(newSettings.passWord.isEmpty())
                    return false;
                isSavePassword = dialog.isSavePassword();
            }
        }
        if(isSavePassword)
        {
            connect(shell, &SshShell::connected, this, [=](){
                    UserAuth::Ptr userAuth(new UserAuth());
                    userAuth->key = UserAuth::hash(newSettings.key());
                    userAuth->auth = newSettings.passWord;
                    UserAuthManager::Instance()->addUserAuth(userAuth);
            });
        }
        shell->connectTo(newSettings);
        settings_ = newSettings;
    }
    return true;
}

void SShWidget::sendCommand(QString const& command)
{
    QStringList commands = command.split("\n");
    sendCommands(commands);
}

void SShWidget::sendCommands(QStringList const& commands)
{
    for(int i = 0; i < commands.size(); i++)
    {
        QString command = commands.at(i);
        if(command.isEmpty())
            continue;
        commandThread_->postCommand(command);
    }
}

void SShWidget::getShellSize(QSize const& size, int &cols, int &rows)
{
    QFont font(console->fontName(), console->fontSize());
    QFontMetricsF fontmetrics(font);
    int w = fontmetrics.width('W');
    int h = fontmetrics.height();
    cols = size.width() / w;
    rows = size.height() / h - 1;
}

QString SShWidget::getTestCommand()
{
    if(testCommands_.isEmpty())
        return QString();
    QString command = testCommands_.front();
    testCommands_.pop_front();
    return command;
}

bool SShWidget::testCommandIsEmpty() const
{
    return testCommands_.isEmpty();
}

void SShWidget::execTestCommand(QString const& command)
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
        if(cmd.startsWith("$") || cmd.startsWith("@"))
            commandThread_->postCommand(cmd);
        else if(!cmd.startsWith("#"))
            execCommand(cmd);
        else
        {
            execExpandCommand(cmd);
            execCommand(QString());
        }
    }
}

bool SShWidget::isConnected() const
{
    return !sheelIsClose;
}

void SShWidget::reconnect(const SSHSettings &settings)
{
    console->clearall();
    shell->reset();
    runShell(settings);
}

void SShWidget::disconnect()
{
    shell->stop();
    sheelIsClose = true;
}

bool SShWidget::isDisplay() const
{
    return dataTimer->isActive();
}

void SShWidget::display()
{
    dataTimer->start(1);
}

void SShWidget::undisplay()
{
    dataTimer->stop();
}

void SShWidget::save()
{
    console->saveToFile();
}

void SShWidget::print()
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

void SShWidget::updateHightLighter(QString const& hightLighter)
{
    console->updateHightLighter(hightLighter);
}

void SShWidget::setHighLighter(QString const& hightLighter)
{
    highLight_ = hightLighter;
}

void SShWidget::copy()
{
    console->copyOne();
}

void SShWidget::copyAll()
{
    console->copyAll();
}

void SShWidget::paste()
{
    console->paste();
}

void SShWidget::setCodecName(QString const& name)
{
    commandParser->setCodecName(name.toUtf8());
}

void SShWidget::setFontName(QString const& name)
{
    console->setFontName(name);
    alternateConsole->setFontName(name);
    getShellSize(consoleSize, shellCols, shellRows);
    emit onSizeChanged(this);
}

void SShWidget::setConsolePalette(ConsolePalette::Ptr palette)
{
    console->setConsolePalette(palette);
    alternateConsole->setConsolePalette(palette);
}

void SShWidget::setConsoleColor(ConsoleColor const& color)
{
    console->setConsoleColor(color);
    alternateConsole->setConsoleColor(color);
}

void SShWidget::setFontSize(int fontSize)
{
    console->setFontSize(fontSize);
    alternateConsole->setFontSize(fontSize);
    getShellSize(consoleSize, shellCols, shellRows);
    emit onSizeChanged(this);
}

void SShWidget::increaseFontSize()
{
    if(console->increaseFontSize())
        emit fontSizeChanged(console->fontSize());
}
void SShWidget::decreaseFontSize()
{
    if(console->decreaseFontSize())
        emit fontSizeChanged(console->fontSize());
}

void SShWidget::clearScrollback()
{
    console->clearall();
    execCommand("clear");
}

void SShWidget::uploadFile()
{
    QString dstPath = getCurrentPath();
    if(dstPath.isEmpty())
        return;

    QString srcFileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    filePath, tr("All files (*.*)"));
    if(srcFileName.isEmpty())
        return;

    QFileInfo fileInfo(srcFileName);
    QString dstFileName = QFileInfo(dstPath, fileInfo.fileName()).filePath();
    filePath = fileInfo.filePath();

    transferFile(srcFileName, dstFileName, true);
}

void SShWidget::downloadFile()
{
    QString srcPath = getCurrentPath();
    if(srcPath.isEmpty())
        return;

    QString fileName = console->selectedText();
    if(fileName.isEmpty())
        fileName = Util::getText(tr("FileName"));
    if(fileName.isEmpty())
           return;

    QString dstPath = QFileDialog::getExistingDirectory(this, tr("Select Path"));
    if(dstPath.isEmpty())
        return;
    filePath = QFileInfo(dstPath).filePath();
    QString srcFileName = QFileInfo(srcPath, fileName).filePath();
    QString dstFileName =  QFileInfo(dstPath, fileName).filePath();
    transferFile(srcFileName, dstFileName, false);
}

QSize SShWidget::sizeHint() const
{
    return QSize(400, 300);
}

void SShWidget::resizeEvent(QResizeEvent *event)
{
    consoleSize = event->size();
    console->resize(consoleSize);
    alternateConsole->resize(consoleSize);
    getShellSize(consoleSize, shellCols, shellRows);
    emit onSizeChanged(this);
}

void SShWidget::activedWidget()
{
    if(shellCols < 0 || shellRows < 0)
        return;

    shell->shellSize(shellCols, shellRows);
    if(!isMainScreen)
        alternateConsole->shellSize(shellCols, shellRows);
}

void SShWidget::closeEvent(QCloseEvent *event)
{
    emit onClose(this);
    event->accept();
    commandThread_->stop();
    commandThread_->wait();
    commandThread_->quit();
}

void SShWidget::connected()
{
    activedWidget();
    shell->run();
    emit onConnectStatus(this, true);
}

void SShWidget::connectionError(QString const& error)
{
    qDebug() << "connectionError: " << error;
    onData(error.toUtf8());
    emit onConnectStatus(this, false);
}

void SShWidget::onData(QByteArray const& data)
{
    if(isTest_)
    {
        testData_.push_back(data);
        if(testData_.contains(testParam_))
        {
            QString command = getTestCommand();
            if(command.startsWith("$") || command.startsWith("@"))
                commandThread_->postCommand(command);
            else if(!command.startsWith("#"))
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
    if(isMainScreen)
        console->putData(data);
    else
        alternateConsole->putData(data);
    if(sheelIsClose)
        sheelIsClose = false;
}

void SShWidget::pullData()
{
    QByteArray data;
    if(shell->read(data))
        onData(data);
}

void SShWidget::secondaryDA()
{
    writeData("\033[>0;95;0c");
}

void SShWidget::onError(QByteArray const& data)
{
    qDebug() << "readyReadStandardError:" << data;
}

void SShWidget::writeData(QByteArray const&data)
{
    shell->write(data);
}

void SShWidget::onGotCursorPos(int row, int col)
{
    QString cursorPos = QString("\033[%1;%2R").arg(row).arg(col);
    writeData(cursorPos.toUtf8());
}

void SShWidget::customContextMenu(const QPoint &)
{
    if(!isMainScreen)
        return;

    QMenu contextMenu;

    contextMenu.addAction(tr("Copy"), this, SLOT(copy()));
    contextMenu.addAction(tr("Copy All"), this, SLOT(copyAll()));
    contextMenu.addAction(tr("Paste"), this, SLOT(paste()));
    contextMenu.addSeparator();
    contextMenu.addAction(tr("Increase Font Size"), this, SLOT(increaseFontSize()));
    contextMenu.addAction(tr("Decrease Font Size"), this, SLOT(decreaseFontSize()));
    contextMenu.addSeparator();
    contextMenu.addAction(tr("Upload..."), this, SLOT(uploadFile()));
    contextMenu.addAction(tr("Download..."), this, SLOT(downloadFile()));
    contextMenu.addSeparator();
    createHighLightMenu(contextMenu.addMenu(tr("Syntax Highlighting")));
    contextMenu.addSeparator();
    contextMenu.addAction(tr("Save To File..."), this, SLOT(save()));
    contextMenu.addAction(tr("Clear Scrollback"), this, SLOT(clearScrollback()));

    contextMenu.exec(QCursor::pos());
    console->cancelSelection();
}

void SShWidget::switchToAlternateScreen()
{
    isMainScreen = false;
    console->hide();
    console->disconnectCommand();
    alternateConsole->connectCommand();
    alternateConsole->connectAppCommand();
    alternateConsole->shellSize(shellCols, shellRows);
    alternateConsole->reset();
    alternateConsole->show();
    alternateConsole->setFocus();
}

void SShWidget::switchToAppKeypadMode()
{
    switchToAlternateScreen();//for vt100
}

void SShWidget::switchToNormalKeypadMode()
{
    if(!isMainScreen)
        switchToMainScreen();
}

void SShWidget::switchToMainScreen()
{
    isMainScreen = true;
    alternateConsole->disconnectCommand();
    alternateConsole->disconnectAppCommand();
    console->connectCommand();
    alternateConsole->hide();
    console->show();
    console->setFocus();
}

void SShWidget::execCommand(QString const& command)
{
    if(!sheelIsClose)
        shell->write(QString("%1\r").arg(command).toUtf8());
}

void SShWidget::execExpandCommand(QString const& command)
{
    QStringList cmds = command.split(' ');
    if(command.startsWith("#bsave"))
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
    else if(command.startsWith("#supload"))
    {
        if(cmds.size() > 2)
            transferFile(cmds[1], cmds[2], true);
    }
    else if(command.startsWith("#sdownload"))
    {
        if(cmds.size() > 2)
            transferFile(cmds[1], cmds[2], false);
    }
    else if(command.startsWith("#esave"))
    {
        afterLogfile_ = LogFile::SharedPtr();
    }
}

void SShWidget::createHighLightMenu(QMenu* menu)
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

void SShWidget::setHighLighter()
{
    QAction* actoin = (QAction*)sender();
    if(actoin)
        emit highLighterChanged(actoin->data().toString());
}

QString SShWidget::getCurrentPath()
{
    int MAX_COUNT = 30;
    QString path;
    QByteArray data;

    undisplay();
    execCommand("pwd");
    for(int i = 0; i < MAX_COUNT; i++)
    {
        QThread::msleep(10);
        QByteArray temp;
        if(shell->read(temp))
            data.push_back(temp);
         QStringList lines = QString::fromUtf8(data).split("\r\n");
         if(lines.size() >= 3)
         {
            path =  lines[1];
            break;
         }
    }
    display();
    return path;
}

void SShWidget::transferFile(QString const& srcFileName, QString const& dstFileName, bool isUpload)
{
    FileProgressDialog dialog(this);
    SftpTransfer transfer(settings_);
    connect(&transfer, &SftpTransfer::gotFileSize, &dialog, &FileProgressDialog::setFileSize);
    connect(&transfer, &SftpTransfer::progressInfo, &dialog, &FileProgressDialog::setProgressInfo);
    connect(&transfer, &SftpTransfer::finished, &dialog, &FileProgressDialog::finished);
    connect(&transfer, &SftpTransfer::error, &dialog, &FileProgressDialog::error);

    if(isUpload)
        dialog.setTitle(tr("Sftp Upload"));
    else
        dialog.setTitle(tr("Sftp Download"));
    dialog.setProtocol("Sftp");
    dialog.setFilename(QFileInfo(srcFileName).fileName());
    dialog.setModal(true);
    dialog.setVisible(true);

    if(isUpload)
        transfer.upload(srcFileName, dstFileName);
    else
        transfer.download(srcFileName, dstFileName);
    while(!dialog.isFinished())
    {
        if(dialog.isCancel())
        {
            transfer.stop();
            while(!dialog.isFinished())
                QApplication::processEvents();
            transfer.stop();
        }
        QApplication::processEvents();
    }
    if(isUpload && dialog.isFinished())
        execCommand(QString("ls -l %1")
                    .arg(QFileInfo(dstFileName).fileName()));
}
