#include "child.h"
#include "console/console.h"
#include "core/commandthread.h"
#include "highlighter/hightlightermanager.h"

#include <QPrinter>
#include <QPrintDialog>
#include <QMenu>
#include <QTimer>

Child::Child(QWidget *parent)
    : QWidget(parent)
    , commandThread_(new CommandThread(this))
    , dataTimer(new QTimer(this))
{
    setAttribute(Qt::WA_DeleteOnClose);

    connect(dataTimer, SIGNAL(timeout()), this, SLOT(pullData()));
    connect(commandThread_, SIGNAL(onAllCommand(QString)), this, SIGNAL(onCommand(QString)));
    connect(commandThread_, SIGNAL(onCommand(QString)), this, SLOT(execCommand(QString)));
    connect(commandThread_, SIGNAL(onExpandCommand(QString)),
            this, SLOT(execExpandCommand(QString)), Qt::BlockingQueuedConnection);
    connect(commandThread_, SIGNAL(onOrderCommandStart(QString)),
            this, SLOT(orderCommandStart(QString)));
    connect(commandThread_, SIGNAL(onOrderCommandEnd()),
            this, SLOT(orderCommandEnd()));

    commandThread_->start();
}

void Child::sendCommand(QString const& command)
{
    QStringList commands = command.split("\n");
    sendCommands(commands);
}

QSize Child::sizeHint() const
{
    return QSize(400, 300);
}

bool Child::isDisplay() const
{
    return dataTimer->isActive();
}

void Child::display()
{
    dataTimer->start(1);
}

void Child::undisplay()
{
    dataTimer->stop();
}

void Child::setCodecName(QString const& name)
{
    console_->setCodecName(name);
}

void Child::setFontName(QString const& name)
{
    console_->setFontName(name);
}

void Child::setConsoleColor(ConsoleColor const& color)
{
   console_->setConsoleColor(color);
}

void Child::setConsolePalette(ConsolePalette::Ptr palette)
{
    console_->setConsolePalette(palette);
}

void Child::setFontSize(int fontSize)
{
    console_->setFontSize(fontSize);
}

void Child::print()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (console_->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted)
        console_->print(&printer);
    delete dlg;
}

void Child::updateHightLighter(QString const& hightLighter)
{
    console_->updateHightLighter(hightLighter);
}

void Child::setHighLighter(QString const& hightLighter)
{
    highLight_ = hightLighter;
}

void Child::save()
{
    console_->saveToFile();
}

void Child::copy()
{
    console_->copyOne();
}

void Child::copyAll()
{
    console_->copyAll();
}

void Child::paste()
{
    console_->paste();
}

void Child::increaseFontSize()
{
    if(console_->increaseFontSize())
        emit fontSizeChanged(console_->fontSize());
}

void Child::decreaseFontSize()
{
    if(console_->decreaseFontSize())
        emit fontSizeChanged(console_->fontSize());
}

void Child::clearScrollback()
{
    console_->clearall();
}

void Child::onExecCommand(QString const& command)
{
    writeToShell(QString("%1\n").arg(command).toUtf8());
}

void Child::resizeEvent(QResizeEvent *event)
{
    console_->resize(event->size());
}

void Child::closeEvent(QCloseEvent *event)
{
    emit onClose(this);
    event->accept();
    commandThread_->stop();
    commandThread_->wait();
    commandThread_->quit();
}

void Child::createConsoleAndConnect()
{
    console_ = createConsole();
    console_->createParserAndConnect();
    connect(console_, SIGNAL(onTitle(QString)), this, SIGNAL(onTitle(QString)));
    connect(console_, SIGNAL(onGotCursorPos(int,int)), this, SLOT(onGotCursorPos(int,int)));
    connect(console_, &Console::getData, this, [=](QByteArray const& data){
        writeToShell(data);
    });
    //connect(console_, SIGNAL(onCommand(QString)), this, SLOT(setCommand(QString)));
    connect(console_, &QWidget::customContextMenuRequested,
            this, &Child::customContextMenu);
}

void Child::onDisplay(QByteArray const& data)
{
    if(isOrderRun)
    {
        cacheData_.push_back(data);
        if(cacheData_.contains(runCommandFlag_))
        {
            cacheData_.clear();
            commandThread_->runOrderCommand();
        }
    }
    console_->putData(data);
}

void Child::setConsoleText(QString const& text)
{
    console_->setPlainText(text);
}

void Child::customContextMenu(const QPoint &)
{
    QMenu contextMenu;

    contextMenu.addAction(tr("Copy"), this, SLOT(copy()));
    contextMenu.addAction(tr("Copy All"), this, SLOT(copyAll()));
    contextMenu.addAction(tr("Paste"), this, SLOT(paste()));
    contextMenu.addSeparator();
    contextMenu.addAction(tr("Increase Font Size"), this, SLOT(increaseFontSize()));
    contextMenu.addAction(tr("Decrease Font Size"), this, SLOT(decreaseFontSize()));
    contextMenu.addSeparator();
    onContextMenu(contextMenu);
    createHighLightMenu(contextMenu.addMenu(tr("Syntax Highlighting")));
    contextMenu.addSeparator();
    contextMenu.addAction(tr("Save To File..."), this, SLOT(save()));
    contextMenu.addAction(tr("Clear Scrollback"), this, SLOT(clearScrollback()));

    contextMenu.exec(QCursor::pos());
    console_->cancelSelection();
}

void Child::setHighLighter()
{
    QAction* actoin = (QAction*)sender();
    if(actoin)
        emit highLighterChanged(actoin->data().toString());
}
void Child::onGotCursorPos(int row, int col)
{
    QString cursorPos = QString("\033[%1;%2R").arg(row).arg(col);
    writeToShell(cursorPos.toUtf8());
}

void Child::execCommand(QString const& command)
{
    onExecCommand(command);
}

void Child::orderCommandStart(QString const& runCommandFlag)
{
    runCommandFlag_ = runCommandFlag.toUtf8();
    isOrderRun = true;
    commandThread_->runOrderCommand();
}

void Child::orderCommandEnd()
{
    isOrderRun = false;
}

void Child::execExpandCommand(QString const& command)
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
                console_->setLogFile(afterLogfile_);
            }
        }
    }
    else if(command.startsWith("#esave"))
    {
        afterLogfile_ = LogFile::SharedPtr();
    }
    onExpandCommand(command);
}

void Child::pullData()
{
    onPullData();
}

void Child::createHighLightMenu(QMenu* menu)
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

void Child::sendCommands(QStringList const& commands)
{
    for(int i = 0; i < commands.size(); i++)
    {
        QString command = commands.at(i);
        if(command.isEmpty())
            continue;
        commandThread_->postCommand(command);
    }
}
