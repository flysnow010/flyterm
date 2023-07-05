#include "telnetwidget.h"
#include "qttelnet.h"
#include "console/telnetconsole.h"
#include "core/telnetsettings.h"
#include "core/commandthread.h"
#include "highlighter/hightlightermanager.h"
#include "util/util.h"

#include <QPrinter>
#include <QPrintDialog>
#include <QMenu>
#include <QResizeEvent>
#include <QTimer>
#include <QtDebug>

TelnetWidget::TelnetWidget(bool isLog, QWidget *parent)
    : QWidget(parent)
    , console(new TelnetConsole(this))
    , commandThread_(new CommandThread(this))
    , telnet(new QtTelnet(this))
{
    setAttribute(Qt::WA_DeleteOnClose);
    if(isLog)
    {
        beforeLogfile_ = LogFile::SharedPtr(new LogFile());
        beforeLogfile_->open(QString("%1/telnet_%2.txt")
                       .arg(Util::logoPath())
                       .arg(uint64_t(this), 8, 16));
    }
    connect(commandThread_, SIGNAL(onAllCommand(QString)), this, SIGNAL(onCommand(QString)));
    connect(commandThread_, SIGNAL(onCommand(QString)), this, SLOT(execCommand(QString)));
    connect(commandThread_, SIGNAL(onExpandCommand(QString)),
            this, SLOT(execExpandCommand(QString)), Qt::BlockingQueuedConnection);
    connect(telnet, SIGNAL(message(QString)), this, SLOT(onMessage(QString)));
    connect(telnet, SIGNAL(loginRequired()), this, SLOT(loginRequired()));
    connect(telnet, SIGNAL(loginFailed()), this, SLOT(loginFailed()));
    connect(telnet, SIGNAL(loggedOut()), this, SLOT(loggedOut()));
    connect(telnet, SIGNAL(loggedIn()), this, SLOT(loggedIn()));
    connect(telnet, SIGNAL(connectionError(QAbstractSocket::SocketError)),
            this, SLOT(connectionError(QAbstractSocket::SocketError)));
    connect(console, SIGNAL(onGotCursorPos(int,int)), this, SLOT(onGotCursorPos(int,int)));
    connect(console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));
    connect(console, &QWidget::customContextMenuRequested,
            this, &TelnetWidget::customContextMenu);
    commandThread_->start();
}

void TelnetWidget::resizeEvent(QResizeEvent *event)
{
    console->resize(event->size());
    QFontMetrics fm(font());
           int lh = fm.lineSpacing();
           int cw = fm.width(QChar('X'));

    telnet->setWindowSize(event->size().width()/ cw, event->size().height() / lh);
}

void TelnetWidget::closeEvent(QCloseEvent *event)
{
    emit onClose(this);
    event->accept();
    commandThread_->stop();
    commandThread_->wait();
    commandThread_->quit();
}

QSize TelnetWidget::sizeHint() const
{
    return QSize(400, 300);
}

QString TelnetWidget::errorString()
{
    QString errorText;
    return errorText;
}

void TelnetWidget::setErrorText(QString const& text)
{
    console->setPlainText(text);
}

void TelnetWidget::disconnect()
{
    telnet->close();
}

void TelnetWidget::save()
{
    console->saveToFile();
}

void TelnetWidget::print()
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

void TelnetWidget::updateHightLighter(QString const& hightLighter)
{
    console->updateHightLighter(hightLighter);
}

void TelnetWidget::setHighLighter(QString const& hightLighter)
{
    highLight_ = hightLighter;
}

void TelnetWidget::copy()
{
    console->copyOne();
}

void TelnetWidget::copyAll()
{
    console->copyAll();
}

void TelnetWidget::paste()
{
    console->paste();
}

void TelnetWidget::setFontName(QString const& name)
{
    console->setFontName(name);
}

void TelnetWidget::setConsoleColor(ConsoleColor const& color)
{
   console->setConsoleColor(color);
}

void TelnetWidget::setConsolePalette(ConsolePalette::Ptr palette)
{
    console->setConsolePalette(palette);
}

void TelnetWidget::setFontSize(int fontSize)
{
    console->setFontSize(fontSize);
}

void TelnetWidget::increaseFontSize()
{
    if(console->increaseFontSize())
        emit fontSizeChanged(console->fontSize());
}
void TelnetWidget::decreaseFontSize()
{
    if(console->decreaseFontSize())
        emit fontSizeChanged(console->fontSize());
}

void TelnetWidget::clearScrollback()
{
    console->clearall();
}

bool TelnetWidget::runShell(TelnetSettings const& settings)
{
    telnet->connectToHost(settings.hostName, settings.port);
    return true;
}

void TelnetWidget::sendCommand(QString const& command)
{
    QStringList commands = command.split("\n");
    sendCommands(commands);
}

void TelnetWidget::sendCommands(QStringList const& commands)
{
    for(int i = 0; i < commands.size(); i++)
    {
        QString command = commands.at(i);
        if(command.isEmpty())
            continue;
        commandThread_->postCommand(command);
    }
}

void TelnetWidget::execCommand(QString const& command)
{
    telnet->sendData(command + QString("\r\n"));
}

void TelnetWidget::execExpandCommand(QString const& command)
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

void TelnetWidget::onMessage(QString const& data)
{
    if(beforeLogfile_)
        beforeLogfile_->write(data);
    console->putData(data.toUtf8());
}

void TelnetWidget::loginRequired()
{
     qDebug() << "loginRequired";
}

void TelnetWidget::loggedIn()
{
    qDebug() << "loggedIn";
}

void TelnetWidget::loggedOut()
{
    qDebug() << "loggedOut";
}

void TelnetWidget::loginFailed()
{
    qDebug() << "loginFailed";
}

void TelnetWidget::connectionError(QAbstractSocket::SocketError error)
{
    qDebug() << QString("Connection error: %1").arg(error);
}

void TelnetWidget::writeData(QByteArray const&data)
{
    telnet->sendData(QString::fromUtf8(data));
}

void TelnetWidget::customContextMenu(const QPoint &)
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

void TelnetWidget::createHighLightMenu(QMenu* menu)
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

void TelnetWidget::setHighLighter()
{
    QAction* actoin = (QAction*)sender();
    if(actoin)
        emit highLighterChanged(actoin->data().toString());
}

void TelnetWidget::onGotCursorPos(int row, int col)
{
    QString cursorPos = QString("\033[%1;%2R").arg(row).arg(col);
    telnet->sendData(cursorPos);
}
