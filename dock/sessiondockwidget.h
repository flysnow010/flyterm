#ifndef SESSIONDOCKWIDGET_H
#define SESSIONDOCKWIDGET_H

#include <QDockWidget>
#include "core/basesession.h"

namespace Ui {
class SessionDockWidget;
}

class QToolBar;
class QTreeView;
class QAction;
class SessionModel;
class SessionDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit SessionDockWidget(QWidget *parent = nullptr);
    ~SessionDockWidget();

    void addSession(Session::Ptr const& session);
    void removeSession(Session::Ptr const& session);
    Session::Ptr findSession(QWidget *widget) { return sessionManager->findSession(widget); }
    void loadSessions();
    void retranslateUi();
signals:
    void onCreateShell(Session::Ptr & session);
    void onCreateSession();
    void onSessionSizeChanged(QWidget* widget);
    void onSessionClose();
public slots:
    void saveSessions();

private slots:
    void onClose(QWidget *widget);
    void createSheel(const QModelIndex &index);
    void customContextMenu(QPoint const& p);
    void delSession();
    void renameSession();
    void upSession();
    void downSession();
    void editSession();
private:
   void createLayout();
   void createToolBar();
   void createTreeView();
   void createConnects();
   void updateTreeView();
   bool isChild(const QModelIndex &index);
private:
    Ui::SessionDockWidget *ui;
    SessionModel* sessionModel;
    SessionManager::Ptr sessionManager;
    QToolBar* toolBar = nullptr;
    QTreeView* treeView = nullptr;
    QAction* addAction = nullptr;
    QAction* delAction = nullptr;
    QAction* renameAction = nullptr;
    QAction* upAction = nullptr;
    QAction* downAction = nullptr;
    QAction* editAction = nullptr;
};

#endif // SESSIONDOCKWIDGET_H
