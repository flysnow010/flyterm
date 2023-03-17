#ifndef COMMANDDIALOG_H
#define COMMANDDIALOG_H

#include <QDialog>

namespace Ui {
class CommandDialog;
}

class CommandDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CommandDialog(QWidget *parent = nullptr);
    ~CommandDialog();

    void setName(QString const& name);
    void setScript(QString const& script);

    QString name() const;
    QString script() const;
private:
    Ui::CommandDialog *ui;
};

#endif // COMMANDDIALOG_H
