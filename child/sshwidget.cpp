#include "sshwidget.h"
#include "console/sshconsole.h"
#include "console/alternateconsole.h"
#include "core/sshsettings.h"
#include "core/sshparser.h"
#include "core/commandthread.h"
#include "core/userauth.h"
#include "dialog/passworddialog.h"
#include "highlighter/hightlightermanager.h"
#include "util/util.h"

#include <QPrinter>
#include <QMenu>
#include <QPrintDialog>
#include <QDebug>

SShWidget::SShWidget(bool isLog, QWidget *parent)
    : QWidget(parent)
    , console(new SshConsole(this))
    , alternateConsole(new AlternateConsole(this))
    , commandThread_(new CommandThread(this))
    , commandParser(new SShParser())
    , shell(new SshShell(this))
{
    setAttribute(Qt::WA_DeleteOnClose);
    alternateConsole->hide();
    console->setCommandParser(commandParser);
    console->connectCommand();
    alternateConsole->setCommandParser(commandParser);
    //alternateConsole->connectAppCommand();
    if(isLog)
    {
        beforeLogfile_ = LogFile::SharedPtr(new LogFile());
        beforeLogfile_->open(QString("%1/ssh_%2_%3.txt")
                       .arg(Util::logoPath())
                       .arg(uint64_t(this), 8, 16)
                       .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss")));
    }

    connect(commandThread_, SIGNAL(onAllCommand(QString)), this, SIGNAL(onCommand(QString)));
    connect(commandThread_, SIGNAL(onCommand(QString)), this, SLOT(execCommand(QString)));
    connect(commandThread_, SIGNAL(onExpandCommand(QString)),
            this, SLOT(execExpandCommand(QString)), Qt::BlockingQueuedConnection);

    connect(shell, SIGNAL(connected()), this, SLOT(connected()));
    connect(shell, SIGNAL(connectionError(QString)), this, SLOT(connectionError(QString)));
    connect(shell, SIGNAL(onData(QByteArray)), this, SLOT(onData(QByteArray)));
    connect(shell, SIGNAL(onError(QByteArray)), this, SLOT(onError(QByteArray)));

    connect(console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));
    connect(console, SIGNAL(onGotCursorPos(int,int)), this, SLOT(onGotCursorPos(int,int)));
    connect(console, SIGNAL(onSwitchToAlternateScreen()), this, SLOT(switchToAlternateCharScreen()));
    connect(console, SIGNAL(onSwitchToAlternateVideoScreen()), this, SLOT(switchToAlternateVideoScreen()));
    connect(console, &QWidget::customContextMenuRequested, this, &SShWidget::customContextMenu);

    connect(alternateConsole, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));
    connect(alternateConsole, SIGNAL(onSwitchToMainScreen()), this, SLOT(switchToMainScreen()));
    connect(alternateConsole, &QWidget::customContextMenuRequested, this, &SShWidget::customContextMenu);
    commandThread_->start();
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
    rows = size.height() / h;
}

void SShWidget::disconnect()
{
    shell->stop();
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

void SShWidget::setFontName(QString const& name)
{
    console->setFontName(name);
    alternateConsole->setFontName(name);
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
}

void SShWidget::activedWidget()
{
    getShellSize(consoleSize, shellCols, shellRows);
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
    shell->run();
}

void SShWidget::connectionError(QString const& error)
{
    qDebug() << "connectionError: " << error;
    onData(error.toUtf8());
}

void SShWidget::onData(QByteArray const& data)
{
    if(beforeLogfile_)
        beforeLogfile_->write(data);
    if(isMainScreen)
        console->putData(data);
    else
        alternateConsole->putData(data);
    if(sheelIsClose)
        sheelIsClose = false;
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

void SShWidget::switchToAlternateScreen(bool isVideo)
{
    isMainScreen = false;
    console->disconnectCommand();
    alternateConsole->connectCommand();
    alternateConsole->connectAppCommand();
    alternateConsole->reset(isVideo);
    console->hide();
    alternateConsole->shellSize(shellCols, shellRows);
    alternateConsole->show();
    alternateConsole->setFocus();
}

void SShWidget::switchToAlternateCharScreen()
{
    switchToAlternateScreen(false);
}

void SShWidget::switchToAlternateVideoScreen()
{
    switchToAlternateScreen(true);
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
        shell->write(QString("%1\n").arg(command).toUtf8());
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


