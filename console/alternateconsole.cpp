#include "alternateconsole.h"
#include "core/consoleparser.h"
#include <QDebug>
#include <QKeyEvent>
#include <QTextFrame>
#include <QTextFrameFormat>
#include <QScrollBar>
#include <QRect>

AlternateConsole::AlternateConsole(QWidget *parent)
    : SshConsole(parent)
{
    document()->setMaximumBlockCount(24);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void AlternateConsole::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Backspace:
        emit getData(e->text().toLocal8Bit());
        break;
    case Qt::Key_Up:
        emit getData("\033OA"); //"\x1B[?25l\x1B[1;23r\x1B[1;1H\x1B[L\x1B[1;24r\x1B[1;1H#ifndef VIDEO_WORKS_FEATURE_H\r\x1B[?12l\x1B[?25h"
        break;
    case Qt::Key_Down:
        emit getData("\033OB"); //"\x1B[?25l\x1B[1;23r\x1B[23;1H\r\n\x1B[1;24r\x1B[23;1H#endif\x1B[24;1H\x1B[K\x1B[23;1H\x1B[?12l\x1B[?25h"
        break;
    case Qt::Key_Right:
        emit getData("\033OC");
        break;
    case Qt::Key_Left:
        emit getData("\033OD");
        break;
    case Qt::Key_Home:
        emit getData("\033OH");
        break;
    case Qt::Key_End:
        emit getData("\033OF");
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        emit getData(e->text().toLocal8Bit());//"\x1B[?25l\x1B[16;23r\x1B[16;1H\x1B[L\x1B[1;24r\x1B[16;1H\x1B[?12l\x1B[?25h"
        break;
    default:
        emit getData(e->text().toLocal8Bit());
        break;
    }
}

void AlternateConsole::connectAppCommand()
{
    if(!commandParser)
        return;

    connect(commandParser, SIGNAL(onSwitchToAppKeypadMode()), this, SLOT(onSwitchToAppKeypadMode()));
    connect(commandParser, SIGNAL(onSwitchToNormalKeypadMode()), this, SLOT(onSwitchToNormalKeypadMode()));
    connect(commandParser, SIGNAL(onASCIIMode()), this, SLOT(onASCIIMode()));
    connect(commandParser, SIGNAL(onDECLineDrawingMode()), this, SLOT(onDECLineDrawingMode()));
    connect(commandParser, SIGNAL(onHideCursor()), this, SLOT(hideCursor()));
    connect(commandParser, SIGNAL(onShowCursor()), this, SLOT(showCursor()));
    connect(commandParser, SIGNAL(onRowRangle(int,int)), this, SLOT(onRowRangle(int,int)));
    connect(commandParser, SIGNAL(onCursorPos(int,int)), this, SLOT(onCursorPos(int,int)));
    connect(commandParser, SIGNAL(onInsertLine(int)), this, SLOT(insertLine(int)));
    connect(commandParser, SIGNAL(onDelCharToLineEnd()), this, SLOT(delCharToLineEnd()));
}

void AlternateConsole::putData(const QByteArray &data)
{
    commandParser->parse(data);
    QScrollBar *bar = verticalScrollBar();
    bar->setValue(0);
}

void AlternateConsole::clearScreen()
{
    screen.clear();
}

void AlternateConsole::putText(QString const& text)
{
    if(text == "\r")
    {
        if(!isPutText)
            home();
        return;
    }
    if(text == "\r\n")
    {
        if(!isPutText)
            down();
        else
        {
            screen.scrollUp(1);
        }
        return;
    }
    if(isPutText)
    {
        screen.setText(text);
        screen.drawText(this, palette_, normalFormat);
    }
    else
    {
        onRight(text.size());
        screen.cursorRight(text.size());
    }
}

void AlternateConsole::setForeColor(ColorRole role)
{
    screen.setForeColor(role);
}

void AlternateConsole::setBackColor(ColorRole role)
{
    screen.setBackColor(role);
}

void  AlternateConsole::setCloseColor()
{
    screen.setForeColor(ColorRole::NullRole);
    screen.setBackColor(ColorRole::NullRole);
}

void AlternateConsole::cursorLeft(int count)
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, count);
    setTextCursor(cursor);
    screen.cursorLeft(count);
}

void AlternateConsole::onSwitchToAppKeypadMode()
{
    qDebug() << "onSwitchToAppKeypadMode";
    int w =  QFontMetrics(QFont(fontFamily(),
                                fontPointSize(),
                                fontWeight())).horizontalAdvance("W");
    setCursorWidth(w + 2);
}

void AlternateConsole::onSwitchToNormalKeypadMode()
{
    qDebug() << "onSwitchToCmdKeypadMode";
}

void AlternateConsole::onASCIIMode()
{
    qDebug() << "onASCIIMode";
    isPutText = true;
    screen.setDrawLineMode(false);
}

void AlternateConsole::onDECLineDrawingMode()
{
    qDebug() << "onDECLineDrawingMode";
    screen.setDrawLineMode(true);
}

void AlternateConsole::onRowRangle(int top, int bottom)
{
    qDebug() << "top:" << top << "bottom: " << bottom;
    topRow = top;
    bottomRow = bottom;
    screen.scrollRangle(top, bottom);
}
void AlternateConsole::hideCursor()
{
    qDebug() << "hideCursor";
    isPutText = true;
}

void AlternateConsole::showCursor()
{
    qDebug() << "showCursor";
    isPutText = false;
}

void AlternateConsole::onCursorPos(int row, int col)
{
    qDebug() << "row:" << row << "col: " << col;
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, row - 1);
    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, col - 1);
    setTextCursor(cursor);
    screen.cursorPos(row, col);
}

void AlternateConsole::insertLine(int lines)
{
    screen.scrollDown(lines);
}

void AlternateConsole::delCharToLineEnd()
{
    onDelCharToLineEnd();
    screen.delCharToLineEnd();
}

void AlternateConsole::home()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfLine);
    setTextCursor(cursor);
    screen.cursorCol(1);
}
void AlternateConsole::down()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::Down);
    setTextCursor(cursor);
    screen.cursorDown(1);
}
