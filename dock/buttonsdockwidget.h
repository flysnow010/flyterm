#ifndef BUTTONSDOCKWIDGET_H
#define BUTTONSDOCKWIDGET_H

#include <QDockWidget>

#include "core/commandmanager.h"
namespace Ui {
class ButtonsDockWidget;
}

class QToolBar;
class QAction;
class ButtonsDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit ButtonsDockWidget(QWidget *parent = nullptr);
    ~ButtonsDockWidget();

    void loadCommands();
    void saveCommands();

    void retranslateUi();
signals:
    void getCommand(QString const& command);

private slots:
    void sendCommand();
    void customContextMenu(const QPoint &pos);
    void newButton();
    void newButtons();
    void renameButtons();
    void deleteButtons();
    void load();
    void save();
    void saveCurrent();
private:
   void createLayout();
   void createToolBar();
   void createActions();
   void updateActions();
   void addCommand(Command::Ptr const& command, QAction* before = 0);
private:
    Ui::ButtonsDockWidget *ui;
    QToolBar* toolBar;
    CommandManager::Ptr commandManger;
    QString filePath;
};

#endif // BUTTONSDOCKWIDGET_H
