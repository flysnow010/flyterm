#ifndef SESSIONMODEL_H
#define SESSIONMODEL_H

#include "treemodel.h"
#include "core/session.h"

class SessionModel : public TreeModel
{
public:
    explicit SessionModel(QObject *parent = nullptr);

    void setSessionManger(SessionManager::Ptr const& sessionManager);
    void update();
    void update(int row, int col);

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
protected:
   TreeItem *createRootItem() override;
   void setupModelData(TreeItem *parent) override;
   bool editable(const QModelIndex &index) const override;
   QVariant decoration(const QModelIndex &index) const override;
private:
   SessionManager::Ptr sessionManager_;
};

#endif // SESSIONMODEL_H
