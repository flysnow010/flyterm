#include "localshellconsole.h"
#include <QDebug>

LocalShellConsole::LocalShellConsole(QWidget *parent)
    : Console(parent)
{
}

void LocalShellConsole::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Backspace:
        emit getData(e->text().toLocal8Bit());
        break;
    case Qt::Key_Left:
        emit getData("\033[D");
        break;
    case Qt::Key_Right:
        emit getData("\033[C");
        break;
    case Qt::Key_Up:
        emit getData("\033[A");
        break;
    case Qt::Key_Down:
        emit getData("\033[B");
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        //onEnd();
        emit getData(e->text().toLocal8Bit());
        if(isNeedNewline())
        {
            emit onCommand(command);
            command = "";
            emit getData("\n");
        }
        break;
    default:
        if (isLocalEchoEnabled())
        {
            QPlainTextEdit::keyPressEvent(e);
            command += e->text();
        }
        emit getData(e->text().toLocal8Bit());
        break;
    }
}
