#include "commandconsole.h"

CommandConsole::CommandConsole(QWidget *parent)
    : QPlainTextEdit(parent)
{
    document()->setMaximumBlockCount(100);
    textFormat.setFontFamily(fontName_);
    textFormat.setFontPointSize(fontSize_);
    mergeCurrentCharFormat(textFormat);
}

void CommandConsole::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Up:
        preCommand();
        break;
    case Qt::Key_Down:
        nextCommand();
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    {
        if(e->modifiers() & Qt::ControlModifier)
        {
            e->setModifiers(Qt::NoModifier);
            QPlainTextEdit::keyPressEvent(e);
        }
        else
            sendCommand();
        break;
    }
    default:
        QPlainTextEdit::keyPressEvent(e);
        break;
    }
}

void CommandConsole::sendCommand()
{
    QString command = QString("%1").arg(toPlainText());
    commands << toPlainText();
    command_index = commands.size();
    setPlainText(QString());
    emit getCommand(command);
}

bool CommandConsole::preCommand()
{
    int index = command_index - 1;
    if(index >= 0&& index < commands.size())
    {
        command_index = index;
        setPlainText(QString());
        insertPlainText(commands.at(index));
        return true;
    }
    return false;
}
bool CommandConsole::nextCommand()
{
    int index = command_index + 1;
    if(index >= 0 && index < commands.size())
    {
        command_index = index;
        setPlainText(QString());
        insertPlainText(commands.at(index));
        return true;
    }
    return false;
}
