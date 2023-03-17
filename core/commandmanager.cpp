#include "commandmanager.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

CommandManager::CommandManager()
{
}

int CommandManager::size() const
{
    return commands_.size();
}

Command::Ptr CommandManager::command(int index)
{
    if(index >=0 && index < commands_.size())
        return commands_.at(index);
    return Command::Ptr();
}

Command::Ptr CommandManager::findCommand(QString const& name)
{
    for(int i= 0; i < commands_.size(); i++)
    {
        if(commands_[i]->name == name)
            return commands_[i];
    }
    return Command::Ptr();
}

void CommandManager::addCommand(Command::Ptr const& command)
{
    commands_.append(command);
}

void CommandManager::removeCommand(Command::Ptr const& command)
{
    commands_.removeAll(command);
}

bool CommandManager::leftCommand(Command::Ptr const& command)
{
    int index = commands_.indexOf(command);
    int newIndex = index - 1;
    if(newIndex >= 0 && index < commands_.size())
    {
        Command::Ptr temp = commands_.at(newIndex);
        commands_[newIndex] = commands_.at(index);
        commands_[index] = temp;
        return true;
    }
    return false;
}

bool CommandManager::rightCommand(Command::Ptr const& command)
{
    int index = commands_.indexOf(command);
    int newIndex = index + 1;
    if(newIndex > 0 && newIndex < commands_.size())
    {
        Command::Ptr temp = commands_.at(newIndex);
        commands_[newIndex] = commands_.at(index);
        commands_[index] = temp;
        return true;
    }
    return false;
}

bool CommandManager::save(QString const& fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
        return false;

    QJsonArray commands;
    for(int i = 0; i < commands_.size(); i++)
    {
        QJsonObject command;
        command.insert("name", commands_[i]->name);
        command.insert("script", commands_[i]->script);
        commands.append(command);
    }

    QJsonObject commandManager;
    commandManager.insert("version", "1.0");
    commandManager.insert("commands", commands);

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

    commands_.clear();
    QJsonObject commandManager = doc.object();
    QJsonArray commands = commandManager.value("commands").toArray();
    for(int i = 0; i < commands.size(); i++)
    {
        QJsonObject object = commands.at(i).toObject();
        Command::Ptr command(new Command);
        command->name = object.value("name").toString();
        command->script = object.value("script").toString();
        commands_ << command;
    }

    return true;
}
