#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H
#include <QString>
#include <QList>

#include <memory>
struct Command
{
    QString name;
    QString script;
    using Ptr = std::shared_ptr<Command>;
};

class CommandManager
{
public:
    CommandManager();
    using Ptr = std::shared_ptr<CommandManager>;

    int size() const;
    Command::Ptr command(int index);
    Command::Ptr findCommand(QString const& name);

    void addCommand(Command::Ptr const& command);
    void removeCommand(Command::Ptr const& command);
    bool leftCommand(Command::Ptr const& command);
    bool rightCommand(Command::Ptr const& command);

    bool save(QString const& fileName);
    bool load(QString const& fileName);
private:
    QList<Command::Ptr> commands_;
};

#endif // COMMANDMANAGER_H
