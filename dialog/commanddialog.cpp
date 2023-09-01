#include "commanddialog.h"
#include "ui_commanddialog.h"
#include "highlighter/scripthighlighter.h"
#include "util/util.h"
#include <QDebug>

bool CommandDialog::isShowMore = false;

CommandDialog::CommandDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CommandDialog)
    , highlighter(new ScriptHighlighter(this))
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    highlighter->setDocument(ui->editScript->document());
    highlighter->rehighlight();
    createConnect();
    showMore(isShowMore);
    highlightCurrentLine();
}

CommandDialog::~CommandDialog()
{
    delete ui;
}

void CommandDialog::setName(QString const& name)
{
    ui->editName->setText(name);
}

void CommandDialog::setScript(QString const& script)
{
    ui->editScript->insertPlainText(script);
}

QString CommandDialog::name() const
{
    return ui->editName->text();
}

QString CommandDialog::script() const
{
    QString text = ui->editScript->toPlainText();
    if(text.endsWith("\n"))
        return text.left(text.size() - 1);
    return text;
}

void CommandDialog::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    QTextEdit::ExtraSelection selection;

    QColor lineColor = QColor(Qt::yellow).lighter(160);

    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = ui->editScript->textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);
    ui->editScript->setExtraSelections(extraSelections);
    updateCommand();
}

void CommandDialog::showMore()
{
    isShowMore = !isShowMore;
    showMore(isShowMore);
}

void CommandDialog::showMore(bool isShow)
{
    if(isShow)
        ui->btnMore->setText(tr("Less"));
    else
        ui->btnMore->setText(tr("More"));

    ui->widSendByKermit->setVisible(isShow);
    ui->widSendByXModem->setVisible(isShow);
    ui->widSendByYModem->setVisible(isShow);
    ui->widSleep->setVisible(isShow);
    ui->widSave->setVisible(isShow);
    ui->widRun->setVisible(isShow);
}

void CommandDialog::updateCommands()
{
    QTextCursor tc = ui->editScript->textCursor();
    tc.movePosition(QTextCursor::Start);
    int lineCount = ui->editScript->blockCount();
    for(int i = 0; i < lineCount; i++)
    {
        QString line = tc.block().text();
        if(!line.isEmpty() && !line.startsWith("!"))
            tc.insertText("!");
        tc.movePosition(QTextCursor::Down);
        tc.movePosition(QTextCursor::StartOfLine);
    }
    ui->editScript->setTextCursor(tc);
}

void CommandDialog::updateCommand()
{
    QString command = ui->editScript->textCursor().block().text();
    if(command.startsWith("#"))
    {
        QStringList commands = command.split(" ");
        if(commands.size() < 2)
            return;
        if(commands[0] == "#ksend")
            ui->lineEditFilenameForKermit->setText(commands[1]);
        else if(commands[0] == "#xsend")
            ui->lineEditFilenameForXModem->setText(commands[1]);
        else if(commands[0] == "#ysend")
            ui->lineEditFilenameForYModem->setText(commands[1]);
        else if(commands[0] == "#bsave")
            ui->lineEditSaveFileName->setText(commands[1]);
    }
    else if(command.startsWith("@"))
    {
        QStringList commands = command.split(" ");
        if(commands.size() < 2)
            return;
        if(commands[0] == "@sleep")
            ui->spinBoxSecond->setValue(commands[1].toInt());
        else if(commands[0] == "@msleep")
            ui->spinBoxMSecond->setValue(commands[1].toInt());
    }
    else if(command.startsWith("!start"))
    {
        QStringList commands = command.split(" ");
        if(commands.size() < 2)
            return;
        ui->lineEditOrderRunPrompt->setText(commands[1]);
    }
}

void CommandDialog::updateCommand(QString const& command)
{
    QTextCursor tc = ui->editScript->textCursor();
    QString line = tc.block().text();
    if(line.isEmpty())
    {
        ui->editScript->insertPlainText(command);
        ui->editScript->insertPlainText("\n");
    }
    else
    {
        QStringList commands = command.split(" ");
        if(line.startsWith(commands[0]))
        {
            tc.select(QTextCursor::LineUnderCursor);
            tc.insertText(command);
        }
    }
}

void CommandDialog::createConnect()
{
    connect(ui->btnBrowseFilenameForKermit, &QToolButton::clicked, this, [=](){
        QString fileName = Util::getOpenFileName("Select file for Kermit Send");
        if(!fileName.isEmpty())
            ui->lineEditFilenameForKermit->setText(fileName);
    });
    connect(ui->btnBrowseFilenameForXModem, &QToolButton::clicked, this, [=](){
        QString fileName = Util::getOpenFileName("Select File for XModem Send");
        if(!fileName.isEmpty())
            ui->lineEditFilenameForXModem->setText(fileName);
    });
    connect(ui->btnBrowseFilenameForYModem, &QToolButton::clicked, this, [=](){
        QString fileName = Util::getOpenFileName("Select file for YModem Send");
        if(!fileName.isEmpty())
            ui->lineEditFilenameForYModem->setText(fileName);
    });
    connect(ui->btnBrowseSaveFilename, &QToolButton::clicked, this, [=](){
        QString fileName = Util::getOpenFileName("Save file");
        if(!fileName.isEmpty())
            ui->lineEditSaveFileName->setText(fileName);
    });
    connect(ui->btnSendByKermit, &QToolButton::clicked, this, [=](){
        QString fileName = ui->lineEditFilenameForKermit->text();
        if(!fileName.isEmpty())
            updateCommand(QString("#ksend %1").arg(fileName));
    });
    connect(ui->btnSendByXModem, &QToolButton::clicked, this, [=](){
        QString fileName = ui->lineEditFilenameForXModem->text();
        if(!fileName.isEmpty())
            updateCommand(QString("#xsend %1").arg(fileName));
    });
    connect(ui->btnSendByYModem, &QToolButton::clicked, this, [=](){
        QString fileName = ui->lineEditFilenameForYModem->text();
        if(!fileName.isEmpty())
            updateCommand(QString("#ysend %1").arg(fileName));
    });
    connect(ui->btnSleep, &QToolButton::clicked, this, [=](){
        updateCommand(QString("@sleep %1").arg(ui->spinBoxSecond->value()));
    });
    connect(ui->btnMSleep, &QToolButton::clicked, this, [=](){
        updateCommand(QString("@msleep %1").arg(ui->spinBoxMSecond->value()));
    });
    connect(ui->btnBeginSave, &QToolButton::clicked, this, [=](){
        QString fileName = ui->lineEditSaveFileName->text();
        if(!fileName.isEmpty())
            updateCommand(QString("#bsave %1").arg(fileName));
    });
    connect(ui->btnEndSave, &QToolButton::clicked, this, [=](){
        ui->editScript->insertPlainText(QString("#esave"));
    });
    connect(ui->btnOrderRun, &QToolButton::clicked, this, [=](){
        QString prompt = ui->lineEditOrderRunPrompt->text();
        if(!prompt.isEmpty())
        {
            updateCommands();
            updateCommand(QString("!start %1").arg(prompt));
        }
    });
    connect(ui->btnMore, &QPushButton::clicked, this, [=](){
        showMore();
    });
    connect(ui->editScript, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
}
