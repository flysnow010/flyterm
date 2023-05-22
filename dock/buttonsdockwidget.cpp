#include "buttonsdockwidget.h"
#include "ui_buttonsdockwidget.h"

#include "dialog/commanddialog.h"

#include "util/util.h"

#include <QVBoxLayout>
#include <QToolBar>
#include <QMenu>
#include <QFileDialog>

ButtonsDockWidget::ButtonsDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ButtonsDockWidget),
    commandManger(new CommandManager())
{
    ui->setupUi(this);
    setTitleBarWidget(new QWidget());

    createLayout();
    createToolBar();
}

ButtonsDockWidget::~ButtonsDockWidget()
{
    delete ui;
}

void ButtonsDockWidget::loadCommands()
{
    QString fileName = QString("%1/commands.json").arg(Util::commandPath());
    commandManger->load(fileName);
    createActions();
}
void ButtonsDockWidget::saveCommands()
{
    QString fileName = QString("%1/commands.json").arg(Util::commandPath());
    commandManger->save(fileName);
}

void ButtonsDockWidget::createLayout()
{
    QHBoxLayout* layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    ui->dockWidgetContents->setLayout(layout);
}

void ButtonsDockWidget::createToolBar()
{
    toolBar = new QToolBar();
    toolBar->setIconSize(QSize(8, 20));
    toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolBar->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(toolBar, &QWidget::customContextMenuRequested,
            this, &ButtonsDockWidget::customContextMenu);
    ui->dockWidgetContents->layout()->addWidget(toolBar);
}

void ButtonsDockWidget::createActions()
{
    for(int i = 0; i < commandManger->size(); i++)
    {
        Command::Ptr command = commandManger->command(i);
        addCommand(command);
    }
}

void ButtonsDockWidget::addCommand(Command::Ptr const& command)
{
    QAction* action = new QAction(QIcon(":image/Tool/dot.png"), command->name);
    action->setToolTip(command->script);
    toolBar->addAction(action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(sendCommand()));
}

void ButtonsDockWidget::sendCommand()
{
    QAction* action = (QAction*)sender();
    QString command = action->toolTip();
    if(!command.endsWith(QChar('\n')))
        command += "\n";
    emit getCommand(command);
}

void ButtonsDockWidget::newButton()
{
    CommandDialog dialog;
    if(dialog.exec() == QDialog::Accepted)
    {
        Command::Ptr command(new Command);
        command->name = dialog.name();
        command->script = dialog.script();
        addCommand(command);
        commandManger->addCommand(command);
    }
}

void ButtonsDockWidget::load()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load Buttons from ..."),
                                                    filePath,
                                                    tr("Buttons (*.json)"));
    if(fileName.isEmpty())
        return;
   filePath = QFileInfo(fileName).path();

   commandManger->load(fileName);
   toolBar->clear();
   createActions();
}

void ButtonsDockWidget::save()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Buttons to ..."),
                               filePath,
                               tr("Buttons (*.json)"));
    if(fileName.isEmpty())
       return;

    filePath = QFileInfo(fileName).path();
    commandManger->save(fileName);
}

void ButtonsDockWidget::customContextMenu(const QPoint &pos)
{
    QAction* action = toolBar->actionAt(pos);
    QMenu contextMenu;
    contextMenu.addAction(tr("New Button"), this, SLOT(newButton()));
    QAction* editAction = contextMenu.addAction(tr("Edit Button"));
    QAction* leftAction = contextMenu.addAction(tr("Move Button Left"));
    QAction* rightAction = contextMenu.addAction(tr("Move Button Right"));
    QAction* deleteAction = contextMenu.addAction(tr("Delete Button"));
    contextMenu.addSeparator();
    contextMenu.addAction(tr("Load Buttons ..."), this, SLOT(load()));
    contextMenu.addAction(tr("Save Buttons ..."), this, SLOT(save()));
    if(action)
    {
        connect(editAction, &QAction::triggered, [=](bool)
        {
            CommandDialog dialog;
            QString name = action->text();

            dialog.setName(name);
            dialog.setScript(action->toolTip());
            if(dialog.exec() == QDialog::Accepted)
            {
                Command::Ptr command = commandManger->findCommand(name);
                if(command)
                {
                    if(command->name != dialog.name())
                    {
                        if(commandManger->findCommand(dialog.name()))
                            return;
                        else
                            command->name = dialog.name();
                    }
                    command->script = dialog.script();
                    action->setText(command->name);
                    action->setToolTip(command->script);
                }
            }
        }
        );

        connect(leftAction, &QAction::triggered, [=](bool)
        {
            QString name = action->text();
            Command::Ptr command = commandManger->findCommand(name);
            if(command && commandManger->leftCommand(command))
            {
                toolBar->clear();
                createActions();
            }
        }
        );

        connect(rightAction, &QAction::triggered, [=](bool)
        {
            QString name = action->text();
            Command::Ptr command = commandManger->findCommand(name);
            if(command && commandManger->rightCommand(command))
            {
                toolBar->clear();
                createActions();
            }
        }
        );

        connect(deleteAction, &QAction::triggered, [=](bool)
        {
            QString name = action->text();
            Command::Ptr command = commandManger->findCommand(name);
            if(command)
            {
                toolBar->removeAction(action);
                commandManger->removeCommand(command);
            }
        }
        );
    }
    else
    {
        editAction->setEnabled(false);
        leftAction->setEnabled(false);
        rightAction->setEnabled(false);
        deleteAction->setEnabled(false);
    }
    contextMenu.exec(QCursor::pos());
}
