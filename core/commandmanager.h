#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H
#include <QString>
#include <QList>

#include <memory>
struct Command
{
    using Ptr = std::shared_ptr<Command>;
    QString name;
    QString script;
};

struct Commands
{
    using Ptr = std::shared_ptr<Commands>;
    QString name;
    QList<Command::Ptr> commands;
    int size() const { return commands.size(); }
};

class CommandManager
{
public:
    CommandManager();
    using Ptr = std::shared_ptr<CommandManager>;

    int size() const;
    Command::Ptr command(int index);
    Command::Ptr findCommand(QString const& name);
    int indexOfCommand(QString const& name);

    void addCommand(Command::Ptr const& command, int index = -1);
    void removeCommand(Command::Ptr const& command);
    bool leftCommand(Command::Ptr const& command);
    bool rightCommand(Command::Ptr const& command);

    bool newCommands(QString const& name);
    bool removeCommands(QString const& name);
    bool setCurrentCommands(QString const& name);
    bool renameCommands(QString const& name, QString const& newName);
    QString currenCommandsName();
    QStringList names() const;

    bool saveCurrentCommands(QString const& fileName);
    bool save(QString const& fileName);
    bool load(QString const& fileName);
private:
    int findCommands(QString const& name);
private:
    QList<Commands::Ptr> commandsList_;
    Commands::Ptr currentCommands_;
};

#endif // COMMANDMANAGER_H
