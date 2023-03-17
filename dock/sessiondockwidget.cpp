#include "sessiondockwidget.h"
#include "ui_sessiondockwidget.h"

#include "model/sessionmodel.h"
#include "core/sshsession.h"
#include "core/serialsession.h"

#include "util/util.h"

#include <QToolBar>
#include <QTreeView>
#include <QHeaderView>

SessionDockWidget::SessionDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::SessionDockWidget),
    sessionModel(new SessionModel(this)),
    sessionManager(new SessionManager())
{
    ui->setupUi(this);
    setTitleBarWidget(new QWidget());
    createLayout();
    createToolBar();
    createTreeView();
    createConnects();
}

SessionDockWidget::~SessionDockWidget()
{
    delete ui;
}

void SessionDockWidget::loadSessions()
{
    QString fileName = QString("%1/sessions.json").arg(Util::sessionPath());
    sessionManager->load(fileName);
    for(int i = 0; i < sessionManager->size(); i++)
    {
        Session::Ptr session = sessionManager->session(i);
        int shellSize = sessionManager->shellSize(i);
        for(int size = 0; size < shellSize; size++)
            emit onCreateShell(session);
        connect(session.get(), &Session::onClose, this, &SessionDockWidget::onClose);
    }
    sessionModel->setSessionManger(sessionManager);
    treeView->setHeaderHidden(true);
    treeView->setModel(sessionModel);
    treeView->header()->resizeSection(0, 255);
    treeView->header()->resizeSection(1, 40);
    treeView->expandAll();
}

void SessionDockWidget::saveSessions()
{
    QString fileName = QString("%1/sessions.json").arg(Util::sessionPath());
    sessionManager->save(fileName);
}

void SessionDockWidget::onClose(QWidget *widget)
{
    Session::Ptr session = sessionManager->findSession(widget);
    if(session){
        session->closeSheel(widget);
        updateTreeView();
    }
}

void SessionDockWidget::createSheel(const QModelIndex &index)
{
    if(index.parent().isValid())
    {
        int row = index.row();
        if(row >= 0 && row < sessionManager->size())
        {
            Session::Ptr session = sessionManager->session(row);
            emit onCreateShell(session);
            updateTreeView();
        }
    }
}

void SessionDockWidget::delSession()
{
    QModelIndex index = treeView->currentIndex();
    if(isChild(index))
    {
        Session::Ptr session = sessionManager->session(index.row());
        if(session && session->shellSize() == 0)
            removeSession(session);
    }
}

void SessionDockWidget::renameSession()
{
    QModelIndex index = treeView->currentIndex();
    if(isChild(index))
    {
        if(index.column() == 0)
            treeView->edit(index);
    }
}

void SessionDockWidget::upSession()
{
    QModelIndex index = treeView->currentIndex();
    if(isChild(index))
    {
        sessionManager->upSession(index.row());
        updateTreeView();
    }
}

void SessionDockWidget::downSession()
{
    QModelIndex index = treeView->currentIndex();
    if(isChild(index))
    {
        sessionManager->downSession(index.row());
        updateTreeView();
    }
}

void SessionDockWidget::editSession()
{
    QModelIndex index = treeView->currentIndex();
    if(!isChild(index))
        return;

    Session::Ptr session = sessionManager->session(index.row());
    session->edit();
}

void SessionDockWidget::addSession(Session::Ptr const& session)
{
    connect(session.get(), &Session::onClose, this, &SessionDockWidget::onClose);
    sessionManager->addSession(session);
    updateTreeView();
}

void SessionDockWidget::removeSession(Session::Ptr const& session)
{
    sessionManager->removeSession(session);
    updateTreeView();
}

void SessionDockWidget::createLayout()
{
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    ui->dockWidgetContents->setLayout(layout);
}

void SessionDockWidget::createToolBar()
{
    toolBar = new QToolBar();
    toolBar->setIconSize(QSize(20, 20));
    ui->dockWidgetContents->layout()->addWidget(toolBar);
    addAction = new QAction(QIcon(":image/Tool/add.png"), "New Session");
    delAction = new QAction(QIcon(":image/Tool/del.png"), "Delete Session");
    renameAction = new  QAction(QIcon(":image/Tool/edit.png"), "Rename Session");
    upAction = new  QAction(QIcon(":image/Tool/up.png"), "Up Session");
    downAction = new  QAction(QIcon(":image/Tool/down.png"), "Down Session");
    editAction = new  QAction("Edit Session");

    toolBar->addAction(addAction);
    toolBar->addAction(delAction);
    toolBar->addAction(upAction);
    toolBar->addAction(downAction);
}

void SessionDockWidget::createTreeView()
{
    treeView = new QTreeView();
    ui->dockWidgetContents->layout()->addWidget(treeView);
    treeView->setContextMenuPolicy(Qt::ActionsContextMenu);

    treeView->addAction(addAction);
    treeView->addAction(delAction);
    treeView->addAction(renameAction);
    treeView->addAction(editAction);
    treeView->addAction(upAction);
    treeView->addAction(downAction);
}

void SessionDockWidget::createConnects()
{
    connect(treeView, &QTreeView::doubleClicked, this, &SessionDockWidget::createSheel);
    connect(addAction, SIGNAL(triggered(bool)), this, SIGNAL(onCreateSession()));
    connect(delAction, SIGNAL(triggered(bool)), this, SLOT(delSession()));
    connect(renameAction, SIGNAL(triggered(bool)), this, SLOT(renameSession()));
    connect(upAction, SIGNAL(triggered(bool)), this, SLOT(upSession()));
    connect(downAction, SIGNAL(triggered(bool)), this, SLOT(downSession()));
    connect(editAction, SIGNAL(triggered(bool)), this, SLOT(editSession()));
}

void SessionDockWidget::updateTreeView()
{
    sessionModel->update();
    treeView->expandAll();
}

bool SessionDockWidget::isChild(const QModelIndex &index)
{
    return index.parent().isValid();
}
