#include <QtGui>
#include <QtGlobal>

#include "treemodel.h"

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent)
    , rootItem(0)
{
}

TreeModel::~TreeModel()
{
    delete rootItem;
}
void TreeModel::reset()
{
    setupData();
}

void TreeModel::setupData()
{
    delete rootItem;
    rootItem = createRootItem();
    setupModelData(rootItem);
    endResetModel();
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if(role == Qt::FontRole)
        return font(index);
    if(role == Qt::DecorationRole)
        return decoration(index);
    if(role == Qt::ForegroundRole)
        return foreColor(index);
    if(role == Qt::BackgroundRole)
        return backColor(index);
    if(role == Qt::TextAlignmentRole)
        return textAlignment(index.column());
    if(role == Qt::UserRole)
        return userData(index);
    if(role == Qt::ToolTipRole)
        return toolTip(index);
    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    if(editable(index))
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);
    if(role == Qt::TextAlignmentRole)
        return textAlignment(section);
    return QVariant();
}

TreeItem* TreeModel::index(QModelIndex const& index) const
{
    return static_cast<TreeItem*>(index.internalPointer());
}
QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

TreeItem* TreeModel::createRootItem()
{
    return 0;
}

QVariant TreeModel::font(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return QVariant();
}

QVariant TreeModel::decoration(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return QVariant();
}

QVariant TreeModel::foreColor(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return QVariant();
}

bool TreeModel::editable(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return false;
}

QVariant TreeModel::backColor(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return QVariant();
}

QVariant TreeModel::textAlignment(int column) const
{
    Q_UNUSED(column)
    return QVariant();
}

QVariant TreeModel::toolTip(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return QVariant();
}

QVariant TreeModel::userData(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return QVariant();
}
