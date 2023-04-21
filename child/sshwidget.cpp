#include "sshwidget.h"
#include "console/sshconsole.h"
#include "console/alternateconsole.h"
#include "core/sshsettings.h"
#include "core/sshparser.h"
#include "core/commandthread.h"
#include "highlighter/hightlightermanager.h"
#include "util/util.h"

#include <sshsettings.h>
#include <QPrinter>
#include <QMenu>
#include <QPrintDialog>

SShWidget::SShWidget(bool isLog, QWidget *parent)
    : QWidget(parent)
    , console(new SshConsole(this))
    , alternateConsole(new AlternateConsole(this))
    , commandThread_(new CommandThread(this))
    , commandParser(new SShParser())
    , shell(new QSsh::SshRemoteProcessRunner(this))
{
    setAttribute(Qt::WA_DeleteOnClose);
    alternateConsole->hide();
    console->setCommandParser(commandParser);
    console->connectCommand();
    alternateConsole->setCommandParser(commandParser);
    alternateConsole->connectAppCommand();
    if(isLog)
    {
        logfile_ = LogFile::Ptr(new LogFile());
        logfile_->open(QString("%1/ssh_%2.txt")
                       .arg(Util::logoPath())
                       .arg(uint64_t(this), 8, 16));
    }

    connect(commandThread_, SIGNAL(onAllCommand(QString)), this, SIGNAL(onCommand(QString)));
    connect(commandThread_, SIGNAL(onCommand(QString)), this, SLOT(execCommand(QString)));
    connect(shell, SIGNAL(connectionError()), this, SLOT(connectionError()));
    connect(shell, SIGNAL(processStarted()), this, SLOT(processStarted()));
    connect(shell, SIGNAL(readyReadStandardOutput()), this, SLOT(readyReadStandardOutput()));
    connect(shell, SIGNAL(readyReadStandardError()), this, SLOT(readyReadStandardError()));
    connect(shell, SIGNAL(processClosed(QString)), this, SLOT(processClosed(QString)));

    connect(console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));
    connect(console, SIGNAL(onSwitchToAlternateScreen()), this, SLOT(switchToAlternateScreen()));
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
void SShWidget::setAskpassFilePath(QString const& filePath)
{
    QSsh::SshSettings::setAskpassFilePath(Utils::FilePath::fromString(filePath));
}

void SShWidget::setSshFilePath(QString const& filePath)
{
    QSsh::SshSettings::setSshFilePath(Utils::FilePath::fromString(filePath));
}

QString SShWidget::sshFilePath()
{
    return QSsh::SshSettings::sshFilePath().toString();
}

bool SShWidget::runShell(SSHSettings const& settings)
{
    QSsh::SshConnectionParameters paramters;

    paramters.setHost(settings.hostName);
    paramters.setUserName(settings.userName);
    paramters.setPort(settings.port);
    if(settings.usePrivateKey)
    {
        paramters.authenticationType = QSsh::SshConnectionParameters::AuthenticationTypeSpecificKey;
        paramters.privateKeyFile = settings.privateKeyFileName;
    }

    shell->runInTerminal(QString(), paramters);
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

void SShWidget::disconnect()
{

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
    console->resize(event->size());
    alternateConsole->resize(event->size());
}

void SShWidget::closeEvent(QCloseEvent *event)
{
    emit onClose(this);
    event->accept();
    commandThread_->stop();
    commandThread_->wait();
    commandThread_->quit();
}

void SShWidget::connectionError()
{
    qDebug() << "connectionError: " << shell->lastConnectionErrorString();
}

void SShWidget::processStarted()
{
}

void SShWidget::readyReadStandardOutput()
{
    QByteArray data = shell->readAllStandardOutput();
    if(logfile_)
        logfile_->write(data);
    if(isMainScreen)
        console->putData(data);
    else
        alternateConsole->putData(data);
    if(sheelIsClose)
        sheelIsClose = false;
}

void SShWidget::readyReadStandardError()
{
    qDebug() << "readyReadStandardError:"
             <<  QString::fromUtf8(shell->readAllStandardError());
}

void SShWidget::processClosed(const QString &error)
{
    sheelIsClose = true;
    console->insertPlainText(error);
}

void SShWidget::writeData(QByteArray const&data)
{
    shell->writeDataToProcess(data);
}

void SShWidget::customContextMenu(const QPoint &)
{
    QMenu contextMenu;

    contextMenu.addAction(tr("Copy"), this, SLOT(copy()));
    contextMenu.addAction(tr("Copy All"), this, SLOT(copyAll()));
    contextMenu.addAction(tr("Paste"), this, SLOT(paste()));
    contextMenu.addSeparator();
    contextMenu.addAction(tr("Increase font size"), this, SLOT(increaseFontSize()));
    contextMenu.addAction(tr("Decrease font size"), this, SLOT(decreaseFontSize()));
    contextMenu.addSeparator();
    createHighLightMenu(contextMenu.addMenu("Syntax Highlighting"));
    contextMenu.addSeparator();
    contextMenu.addAction(tr("Save to file"), this, SLOT(save()));
    contextMenu.addAction(tr("Clear Scrollback"), this, SLOT(clearScrollback()));

    contextMenu.exec(QCursor::pos());
    console->cancelSelection();
}

void SShWidget::switchToAlternateScreen()
{
    isMainScreen = false;
    console->disconnectCommand();
    alternateConsole->connectCommand();
    alternateConsole->clearScreen();
    console->hide();
    alternateConsole->show();
    alternateConsole->setFocus();
}

void SShWidget::switchToMainScreen()
{
    isMainScreen = true;
    alternateConsole->disconnectCommand();
    console->connectCommand();
    alternateConsole->hide();
    console->show();
    console->setFocus();
}

void SShWidget::execCommand(QString const& command)
{
    if(!sheelIsClose)
        shell->writeDataToProcess(QString("%1\n").arg(command).toUtf8());
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


