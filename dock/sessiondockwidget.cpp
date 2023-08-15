#include "sessiondockwidget.h"
#include "ui_sessiondockwidget.h"

#include "model/sessionmodel.h"
#include "core/sshsession.h"
#include "core/serialsession.h"

#include "util/util.h"

#include <QToolBar>
#include <QTreeView>
#include <QMenu>
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
        connect(session.get(), &Session::onSizeChanged, this,
                &SessionDockWidget::onSessionSizeChanged);
    }
    sessionModel->setSessionManger(sessionManager);
    treeView->setHeaderHidden(false);
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

void SessionDockWidget::retranslateUi()
{
    ui->retranslateUi(this);
    addAction->setText(tr("New Session"));
    delAction->setText(tr("Delete Session"));
    renameAction->setText(tr("Rename Session"));
    upAction->setText(tr("Up Session"));
    downAction->setText(tr("Down Session"));
    editAction->setText(tr("Edit Session"));
    sessionModel->update();
    treeView->expandAll();
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
    connect(session.get(), &Session::onSizeChanged,
            this, &SessionDockWidget::onSessionSizeChanged);
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
    toolBar->setIconSize(QSize(16, 16));
    ui->dockWidgetContents->layout()->addWidget(toolBar);
    addAction = new QAction(QIcon(":image/Tool/add.png"), tr("New Session"));
    delAction = new QAction(QIcon(":image/Tool/del.png"), tr("Delete Session"));
    renameAction = new  QAction(QIcon(":image/Tool/edit.png"), tr("Rename Session"));
    upAction = new  QAction(QIcon(":image/Tool/up.png"), tr("Up Session"));
    downAction = new  QAction(QIcon(":image/Tool/down.png"), tr("Down Session"));
    editAction = new  QAction(tr("Edit Session"));

    toolBar->addAction(addAction);
    toolBar->addAction(delAction);
    toolBar->addAction(upAction);
    toolBar->addAction(downAction);
}

void SessionDockWidget::createTreeView()
{
    treeView = new QTreeView();
    treeView->setRootIsDecorated(false);
    treeView->setAlternatingRowColors(true);
    treeView->setAllColumnsShowFocus(true);

    ui->dockWidgetContents->layout()->addWidget(treeView);
    treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(treeView, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(customContextMenu(QPoint)));
}

void SessionDockWidget::customContextMenu(QPoint const& p)
{
    QModelIndex index = treeView->indexAt(p);
    QMenu menu;
    if(!index.isValid() || !index.parent().isValid())
        menu.addAction(addAction);
    else
    {
        menu.addAction(addAction);
        menu.addAction(delAction);
        menu.addAction(renameAction);
        menu.addAction(editAction);
        menu.addSeparator();
        menu.addAction(upAction);
        menu.addAction(downAction);
    }

    menu.exec(QCursor::pos());
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
