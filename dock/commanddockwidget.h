#ifndef COMMANDDOCKWIDGET_H
#define COMMANDDOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
class CommandDockWidget;
}

class CommandConsole;
class HistoryConsole;
class QSyntaxHighlighter;
class CommandDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit CommandDockWidget(QWidget *parent = nullptr);
    ~CommandDockWidget();

signals:
    void getCommand(QString const& command);
public slots:
    void addToHistory(QString const& command);
private slots:
    void customContextMenu(const QPoint &);
    void save();
    void load();
    void exec();
private:
   void createLayout();
   void createConsole();
   void createHistory();
private:
    Ui::CommandDockWidget *ui;
    QSyntaxHighlighter *highlighter;
    CommandConsole* console;
    HistoryConsole*   history;
    QString filePath;
};

#endif // COMMANDDOCKWIDGET_H
