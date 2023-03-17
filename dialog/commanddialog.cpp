#include "commanddialog.h"
#include "ui_commanddialog.h"

CommandDialog::CommandDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CommandDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
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
    return ui->editScript->toPlainText();
}
