#include "wslconsole.h"
#include "core/sshparser.h"

#include <QDebug>

WSLConsole::WSLConsole(QWidget *parent)
    : Console(parent)
{
    cursorWidth_ = cursorWidth();
}

void WSLConsole::connectCommands()
{
    Console::connectCommands();
    connect(parser(), SIGNAL(onHideCursor()), this, SLOT(hideCursor()));
    connect(parser(), SIGNAL(onCleanScreen()), this, SLOT(cleanScreen()));
    connect(parser(), SIGNAL(onScreenHome()), this, SLOT(screenHome()));
    connect(parser(), SIGNAL(onTitle(QString)), this, SIGNAL(onTitle(QString)));
    connect(parser(), SIGNAL(onRight(int)), this, SLOT(right(int)));
    connect(parser(), SIGNAL(onShowCursor()), this, SLOT(showCursor()));
    connect(parser(), SIGNAL(onEraseChars(int)), this, SLOT(eraseChars(int)));
    connect(parser(), SIGNAL(onCursorPos(int,int)), this, SLOT(cursorPos(int,int)));
    //debug(true);
}

void WSLConsole::disconnectCommands()
{
    Console::disconnectCommands();
    disconnect(parser(), SIGNAL(onHideCursor()), this, SLOT(hideCursor()));
    disconnect(parser(), SIGNAL(onCleanScreen()), this, SLOT(cleanScreen()));
    disconnect(parser(), SIGNAL(onScreenHome()), this, SLOT(screenHome()));
    disconnect(parser(), SIGNAL(onTitle(QString)), this, SIGNAL(onTitle(QString)));
    disconnect(parser(), SIGNAL(onRight(int)), this, SLOT(right(int)));
    disconnect(parser(), SIGNAL(onShowCursor()), this, SLOT(showCursor()));
    disconnect(parser(), SIGNAL(onEraseChars(int)), this, SLOT(eraseChars(int)));
    disconnect(parser(), SIGNAL(onCursorPos(int,int)), this, SLOT(cursorPos(int,int)));
}

ConsoleParser* WSLConsole::createParser()
{
    return new SShParser();
}

void WSLConsole::hideCursor()
{
    if(isDebug())
        qDebug() << "hideCursor";
    setCursorWidth(0);
}

void WSLConsole::cleanScreen()
{
    if(isDebug())
        qDebug() << "cleanScreen";
    clear();
}

void WSLConsole::screenHome()
{
    if(isDebug())
        qDebug() << "screenHome";
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::Start);
    setTextCursor(cursor);
}

void  WSLConsole::cursorPos(int row, int col)
{
    if(isDebug())
    {
        qDebug() << "cursorPos(" << row << "," << col << ")";
        qDebug() << "blockCount: " << blockCount();
    }
    if(row > blockCount())
    {
        QString lines(1, QChar('\n'));
        insertPlainText(lines);
    }
    else
    {
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::Start);
        cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, row);
        cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, col);
        setTextCursor(cursor);
    }
}

void WSLConsole::eraseChars(int n)
{
    if(isDebug())
        qDebug() << "eraseChars(" << n << ")";
    QTextCursor cursor = textCursor();
    for(int i = 0; i < n; i++)
        cursor.deleteChar();
}

void WSLConsole::right(int n)
{
    if(isDebug())
        qDebug() << "right(" << n << ")";
    QTextCursor cursor = textCursor();
    if(!cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, n))
    {
        QString spaces(n, QChar(' '));
        insertPlainText(spaces);
    }
}

void WSLConsole::showCursor()
{
    if(isDebug())
        qDebug() << "showCursor";
    setCursorWidth(cursorWidth_);
}
