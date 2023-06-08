#include "commandmanager.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

CommandManager::CommandManager()
{
}

int CommandManager::size() const
{
    return currentCommands_->size();
}

Command::Ptr CommandManager::command(int index)
{
    if(index >=0 && index < currentCommands_->size())
        return currentCommands_->commands.at(index);
    return Command::Ptr();
}

Command::Ptr CommandManager::findCommand(QString const& name)
{
    for(int i= 0; i < currentCommands_->size(); i++)
    {
        if(currentCommands_->commands[i]->name == name)
            return currentCommands_->commands[i];
    }
    return Command::Ptr();
}

void CommandManager::addCommand(Command::Ptr const& command)
{
    currentCommands_->commands.append(command);
}

void CommandManager::removeCommand(Command::Ptr const& command)
{
    currentCommands_->commands.removeAll(command);
}

bool CommandManager::leftCommand(Command::Ptr const& command)
{
    int index = currentCommands_->commands.indexOf(command);
    int newIndex = index - 1;
    if(newIndex >= 0 && index < currentCommands_->size())
    {
        Command::Ptr temp = currentCommands_->commands.at(newIndex);
        currentCommands_->commands[newIndex] = currentCommands_->commands.at(index);
        currentCommands_->commands[index] = temp;
        return true;
    }
    return false;
}

bool CommandManager::rightCommand(Command::Ptr const& command)
{
    int index = currentCommands_->commands.indexOf(command);
    int newIndex = index + 1;
    if(newIndex > 0 && newIndex < currentCommands_->size())
    {
        Command::Ptr temp = currentCommands_->commands.at(newIndex);
        currentCommands_->commands[newIndex] = currentCommands_->commands.at(index);
        currentCommands_->commands[index] = temp;
        return true;
    }
    return false;
}

bool CommandManager::save(QString const& fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
        return false;

    QJsonArray items;
    for(int i = 0; i < commandsList_.size(); i++)
    {
        QJsonObject item;
        QJsonArray commands;
        item.insert("name", commandsList_[i]->name);
        for(int j = 0; j < commandsList_[i]->size(); j++)
        {
            QJsonObject command;
            command.insert("name", commandsList_[i]->commands[j]->name);
            command.insert("script", commandsList_[i]->commands[j]->script);
            commands.append(command);
        }
        item.insert("commands", commands);
        items.append(item);
    }

    QJsonObject commandManager;
    commandManager.insert("version", "2.0");
    commandManager.insert("items", items);

    QJsonDocument doc;
    doc.setObject(commandManager);
    file.write(doc.toJson());

    return true;
}

bool CommandManager::load(QString const& fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
        return false;

    QJsonParseError json_error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &json_error);
    if(json_error.error != QJsonParseError::NoError)
        return false;

    QJsonObject commandManager = doc.object();
    QString version = commandManager.value("version").toString();
    if(version == "1.0")
    {
        QJsonArray commands = commandManager.value("commands").toArray();
        Commands::Ptr commandsPtr(new Commands());
        commandsPtr->name = "Default";
        for(int i = 0; i < commands.size(); i++)
        {
            QJsonObject object = commands.at(i).toObject();
            Command::Ptr command(new Command);
            command->name = object.value("name").toString();
            command->script = object.value("script").toString();
            commandsPtr->commands << command;
        }
        commandsList_ << commandsPtr;
    }
    else if(version == "2.0")
    {
         QJsonArray items = commandManager.value("items").toArray();
         for(int i = 0; i < items.size(); i++)
         {
            QJsonObject item = items.at(i).toObject();
            Commands::Ptr commandsPtr(new Commands());
            commandsPtr->name = item.value("name").toString();
            QJsonArray commands = item.value("commands").toArray();
            for(int j = 0; j < commands.size(); j++)
            {
                QJsonObject object = commands.at(j).toObject();
                Command::Ptr command(new Command);
                command->name = object.value("name").toString();
                command->script = object.value("script").toString();
                commandsPtr->commands << command;
            }
            commandsList_ << commandsPtr;
         }
    }
    if(commandsList_.isEmpty())
        currentCommands_ = Commands::Ptr(new Commands());
    else
        currentCommands_ = commandsList_[0];

    return true;
}
