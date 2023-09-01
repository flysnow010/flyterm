#ifndef COMMANDDIALOG_H
#define COMMANDDIALOG_H

#include <QDialog>

namespace Ui {
class CommandDialog;
}
class QSyntaxHighlighter;
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
private slots:
    void highlightCurrentLine();
    void showMore();
private:
    void createConnect();
    void updateCommands();
    void updateCommand();
    void updateCommand(QString const& command);
    void showMore(bool isShow);
private:
    Ui::CommandDialog *ui;
    QSyntaxHighlighter *highlighter;
    static bool isShowMore;
};

#endif // COMMANDDIALOG_H
