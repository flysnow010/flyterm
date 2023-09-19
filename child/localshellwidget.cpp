#include "localshellwidget.h"
#include "console/localshellconsole.h"
#include "core/commandthread.h"
#include "core/localshell.h"
#include "core/localshellsettings.h"
#include "highlighter/hightlightermanager.h"
#include "util/util.h"

#include <QPrinter>
#include <QPrintDialog>
#include <QMenu>
#include <QResizeEvent>
#include <QTimer>
#include <QTimer>

LocalShellWidget::LocalShellWidget(bool isLog, QWidget *parent)
    : QWidget(parent)
    , console(new LocalShellConsole(this))
    , commandThread(new CommandThread(this))
    , shell(new LocalShell(this))
    , dataTimer(new QTimer(this))
{
    setAttribute(Qt::WA_DeleteOnClose);
    console->createParserAndConnect();
    if(isLog)
    {
        beforeLogfile_ = LogFile::SharedPtr(new LogFile());
        beforeLogfile_->open(QString("%1/telnet_%2_%3.txt")
                       .arg(Util::logoPath())
                       .arg(uint64_t(this), 8, 16)
                       .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss")));
    }
    connect(dataTimer, SIGNAL(timeout()), this, SLOT(pullData()));
    connect(commandThread, SIGNAL(onAllCommand(QString)), this, SIGNAL(onCommand(QString)));
    connect(commandThread, SIGNAL(onCommand(QString)), this, SLOT(execCommand(QString)));
    connect(commandThread, SIGNAL(onExpandCommand(QString)),
            this, SLOT(execExpandCommand(QString)), Qt::BlockingQueuedConnection);
    connect(commandThread, SIGNAL(onTestCommand(QString)),
            this, SLOT(execTestCommand(QString)));
    connect(shell, SIGNAL(onData(QByteArray)), this, SLOT(onData(QByteArray)));
    connect(console, SIGNAL(onGotCursorPos(int,int)), this, SLOT(onGotCursorPos(int,int)));
    connect(console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));
    connect(console, SIGNAL(onCommand(QString)), this, SLOT(setCommand(QString)));
    connect(console, &QWidget::customContextMenuRequested,
            this, &LocalShellWidget::customContextMenu);
    commandThread->start();
    dataTimer->start(1);
}

bool LocalShellWidget::runShell(LocalShellSettings const& settings)
{
    if(settings.shellType == "cmd")
        return runCmdShell(settings);
    else if(settings.shellType == "powershell")
        return runPowerShell(settings);
    return false;
}

bool LocalShellWidget::runCmdShell(LocalShellSettings const& settings)
{
    console->setLocalEchoEnabled(true);
    console->setNeedNewLine(true);
    if(settings.currentPath.isEmpty())
        shell->start(settings.shellType);
    else
    {
        QStringList params;
        params << "/F:ON" << "/E:ON" << "/s" <<  "/k" << "pushd" << settings.getCurrentPath();
        shell->start(settings.shellType, params);
    }
    isConnected_ = true;
    return true;
}

bool LocalShellWidget::runPowerShell(LocalShellSettings const& settings)
{
    QStringList params;
    //params << "-version" << "5"; 3,4,5
    params << "-nologo";
    if(!settings.currentPath.isEmpty())
    {
        params << "-noexit" <<  "-command" << "Set-Location"
               << "-literalPath" << settings.getCurrentPath();
    }
    shell->start(settings.shellType, params);
    isConnected_ = true;
    return true;
}

void LocalShellWidget::sendCommand(QString const& command)
{
    QStringList commands = command.split("\n");
    sendCommands(commands);
}

QSize LocalShellWidget::sizeHint() const
{
    return QSize(400, 300);
}

bool LocalShellWidget::isConnected() const
{
    return isConnected_;
}

void LocalShellWidget::reconnect(LocalShellSettings const& settings)
{
    console->clearall();
    runShell(settings);
}


void LocalShellWidget::disconnect()
{
    //telnet->close();
    isConnected_ = false;
}

bool LocalShellWidget::isDisplay() const
{
    return dataTimer->isActive();
}

void LocalShellWidget::display()
{
    dataTimer->start(1);
}

void LocalShellWidget::undisplay()
{
    dataTimer->stop();
}

void LocalShellWidget::setCodecName(QString const& name)
{
    console->setCodecName(name);
}

void LocalShellWidget::setFontName(QString const& name)
{
    console->setFontName(name);
}

void LocalShellWidget::setConsoleColor(ConsoleColor const& color)
{
   console->setConsoleColor(color);
}

void LocalShellWidget::setConsolePalette(ConsolePalette::Ptr palette)
{
    console->setConsolePalette(palette);
}

void LocalShellWidget::setFontSize(int fontSize)
{
    console->setFontSize(fontSize);
}

void LocalShellWidget::print()
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

void LocalShellWidget::updateHightLighter(QString const& hightLighter)
{
    console->updateHightLighter(hightLighter);
}

void LocalShellWidget::setHighLighter(QString const& hightLighter)
{
    highLight_ = hightLighter;
}


void LocalShellWidget::save()
{
    console->saveToFile();
}

void LocalShellWidget::copy()
{
    console->copyOne();
}

void LocalShellWidget::copyAll()
{
    console->copyAll();
}

void LocalShellWidget::paste()
{
    console->paste();
}

void LocalShellWidget::increaseFontSize()
{
    if(console->increaseFontSize())
        emit fontSizeChanged(console->fontSize());
}
void LocalShellWidget::decreaseFontSize()
{
    if(console->decreaseFontSize())
        emit fontSizeChanged(console->fontSize());
}

void LocalShellWidget::clearScrollback()
{
    console->clearall();
}

void LocalShellWidget::resizeEvent(QResizeEvent *event)
{
    console->resize(event->size());
}

void LocalShellWidget::closeEvent(QCloseEvent *event)
{
    emit onClose(this);
    event->accept();
    shell->stop();
    commandThread->stop();
    commandThread->wait();
    commandThread->quit();
}

void LocalShellWidget::onData(QByteArray const& data)
{
    QString text = QString::fromLocal8Bit(data);
    if(!text.isEmpty() && text.startsWith(command_))
    {
        text = text.mid(command_.size());
        command_ = "";
    }
    texts << text;
}

void LocalShellWidget::writeData(QByteArray const&data)
{
    shell->write(data);
}

void LocalShellWidget::customContextMenu(const QPoint &)
{
    QMenu contextMenu;

    contextMenu.addAction(tr("Copy"), this, SLOT(copy()));
    contextMenu.addAction(tr("Copy All"), this, SLOT(copyAll()));
    contextMenu.addAction(tr("Paste"), this, SLOT(paste()));
    contextMenu.addSeparator();
    contextMenu.addAction(tr("Increase Font Size"), this, SLOT(increaseFontSize()));
    contextMenu.addAction(tr("Decrease Font Size"), this, SLOT(decreaseFontSize()));
    contextMenu.addSeparator();
    createHighLightMenu(contextMenu.addMenu(tr("Syntax Highlighting")));
    contextMenu.addSeparator();
    contextMenu.addAction(tr("Save To File..."), this, SLOT(save()));
    contextMenu.addAction(tr("Clear Scrollback"), this, SLOT(clearScrollback()));

    contextMenu.exec(QCursor::pos());
    console->cancelSelection();
}

void LocalShellWidget::setHighLighter()
{
    QAction* actoin = (QAction*)sender();
    if(actoin)
        emit highLighterChanged(actoin->data().toString());
}

void LocalShellWidget::onGotCursorPos(int row, int col)
{
    QString cursorPos = QString("\033[%1;%2R").arg(row).arg(col);
    shell->write(cursorPos.toUtf8());
}

void LocalShellWidget::execCommand(QString const& command)
{
    shell->write(QString("%1\n").arg(command).toUtf8());
}

void LocalShellWidget::execTestCommand(QString const& command)
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

void LocalShellWidget::execExpandCommand(QString const& command)
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
    else if(command.startsWith("#esave"))
    {
        afterLogfile_ = LogFile::SharedPtr();
    }
}

void LocalShellWidget::pullData()
{
    if(texts.isEmpty())
        return;

    QString data;
    while(texts.size() > 0 && data.size() < 512)
        data.push_back(texts.takeFirst());

    if(isTest_)
    {
        testData_.push_back(data.toUtf8());
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
    console->putData(data.toUtf8());
}

void LocalShellWidget::createHighLightMenu(QMenu* menu)
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

void LocalShellWidget::sendCommands(QStringList const& commands)
{
    for(int i = 0; i < commands.size(); i++)
    {
        QString command = commands.at(i);
        if(command.isEmpty())
            continue;
        commandThread->postCommand(command);
    }
}

QString LocalShellWidget::getTestCommand()
{
    if(testCommands_.isEmpty())
        return QString();
    QString command = testCommands_.front();
    testCommands_.pop_front();
    return command;
}

bool LocalShellWidget::testCommandIsEmpty() const
{
    return testCommands_.isEmpty();
}

void LocalShellWidget::setCommand(QString const& command)
{
    command_ = command;
}
