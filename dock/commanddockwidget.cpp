#include "commanddockwidget.h"
#include "ui_commanddockwidget.h"

#include "console/commandconsole.h"
#include "console/historyconsole.h"
#include "highlighter/scripthighlighter.h"
#include "util/util.h"

#include <QScrollBar>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMenu>

CommandDockWidget::CommandDockWidget(QWidget *parent)
    : QDockWidget(parent)
    , ui(new Ui::CommandDockWidget)
    , highlighter(new ScriptHighlighter(this))
    , console(0)
    , history(0)
{
    ui->setupUi(this);
    setTitleBarWidget(new QWidget());

    createLayout();
    createConsole();
    createHistory();
}

CommandDockWidget::~CommandDockWidget()
{
    delete ui;
}

void CommandDockWidget::createLayout()
{
    QHBoxLayout* layout = new QHBoxLayout();
    layout->setContentsMargins(0 ,1, 0, 0);
    layout->setSpacing(0);
    ui->dockWidgetContents->setLayout(layout);
}

void CommandDockWidget::createConsole()
{
    console = new CommandConsole();
    ui->dockWidgetContents->layout()->addWidget(console);
    connect(console, &CommandConsole::getCommand, this, &CommandDockWidget::getCommand);
}

void CommandDockWidget::createHistory()
{
    history = new HistoryConsole();
    connect(history, &QWidget::customContextMenuRequested, this, &CommandDockWidget::customContextMenu);
    ui->dockWidgetContents->layout()->addWidget(history);
    highlighter->setDocument(history->document());
    highlighter->rehighlight();
}

void CommandDockWidget::addToHistory(QString const& command)
{
    history->append(command);
    QScrollBar *bar = history->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void CommandDockWidget::customContextMenu(const QPoint &)
{
    QMenu contextMenu;

    contextMenu.addAction(tr("Copy"), history, SLOT(copy()), QKeySequence::fromString("Ctrl+C"));
    contextMenu.addSeparator();
    contextMenu.addAction(tr("Execue Command"), this, SLOT(exec()))->setEnabled(
                !history->textCursor().selectedText().isEmpty());
    contextMenu.addSeparator();
    contextMenu.addAction(tr("Save To File..."), this, SLOT(save()));
    contextMenu.addAction(tr("Load From File..."), this, SLOT(load()));
    contextMenu.addAction(tr("Select All"), history, SLOT(selectAll()), QKeySequence::fromString("Ctrl+A"));
    contextMenu.exec(QCursor::pos());
}

void CommandDockWidget::retranslateUi()
{
    ui->retranslateUi(this);
}

void CommandDockWidget::save()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save To"),
                               filePath,
                               tr("History file (*.txt)"));
    if(fileName.isEmpty())
       return;

    filePath = QFileInfo(fileName).path();
    history->saveToFile(fileName);
}

void CommandDockWidget::load()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load From"),
                                                    filePath,
                                                    tr("History file (*.txt)"));
    if(fileName.isEmpty())
        return;
   filePath = QFileInfo(fileName).path();
   history->loadFromFile(fileName);
}

void CommandDockWidget::exec()
{
    QString command = history->textCursor().selectedText();
    emit getCommand(command);

}
