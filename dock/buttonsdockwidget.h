#ifndef BUTTONSDOCKWIDGET_H
#define BUTTONSDOCKWIDGET_H

#include <QDockWidget>

#include "core/commandmanager.h"
namespace Ui {
class ButtonsDockWidget;
}

class QToolBar;
class ButtonsDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit ButtonsDockWidget(QWidget *parent = nullptr);
    ~ButtonsDockWidget();

    void loadCommands();
    void saveCommands();
signals:
    void getCommand(QString const& command);

private slots:
    void sendCommand();
    void customContextMenu(const QPoint &pos);
    void newButton();
    void newButtons();
    void load();
    void save();
private:
   void createLayout();
   void createToolBar();
   void createActions();
   void addCommand(Command::Ptr const& command);
private:
    Ui::ButtonsDockWidget *ui;
    QToolBar* toolBar;
    CommandManager::Ptr commandManger;
    QString filePath;
};

#endif // BUTTONSDOCKWIDGET_H
