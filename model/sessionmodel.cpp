#include "sessionmodel.h"

SessionModel::SessionModel(QObject *parent)
    : TreeModel(parent)
{

}

void SessionModel::setSessionManger(SessionManager::Ptr const& sessionManager)
{
    sessionManager_ = sessionManager;
    reset();
}

void SessionModel::update()
{
    reset();
}

void SessionModel::update(int row, int col)
{
    emit dataChanged(index(row, col), index(row, col));
}

TreeItem *SessionModel::createRootItem()
{
    return new TreeItem(QList<QVariant>() << "" << "");
}

bool SessionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.column() == 0
        && index.parent().isValid()
        && role == Qt::EditRole)
    {
        QString name = value.toString();
        if(!name.isEmpty())
         {
            Session::Ptr session = sessionManager_->session(index.row());
            session->setName(name);
            TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
            return item->setData(index.column(), value);
        }
    }
    return false;
}

void SessionModel::setupModelData(TreeItem *parent)
{
    if(!sessionManager_)
        return;

    TreeItem* root = new TreeItem(QList<QVariant>() << tr("Sessions") << "", parent);
    for(int i = 0; i < sessionManager_->size(); i++)
    {
        Session::Ptr session = sessionManager_->session(i);
        QString name = session->name();
        int size = session->shellSize();
        TreeItem* child = new TreeItem(QList<QVariant>() << name << size, root);
        root->appendChild(child);
    }
    parent->appendChild(root);
}

QVariant SessionModel::decoration(const QModelIndex &index) const
{
    if(index.column() == 0 && index.parent().isValid())
    {
        Session::Ptr session = sessionManager_->session(index.row());
        return session->icon();
    }
    return QVariant();
}

bool SessionModel::editable(const QModelIndex &index) const
{
    if(index.column() == 0 && index.parent().isValid())
        return true;
    return false;
}
