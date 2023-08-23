#include "alternateconsole.h"
#include "core/consoleparser.h"
#include <QDebug>
#include <QKeyEvent>
#include <QTextFrame>
#include <QTextFrameFormat>
#include <QScrollBar>
#include <QRect>

#ifdef DEBUG
#define SHOW_INFO
#endif
#define NEW_DRAW
AlternateConsole::AlternateConsole(QWidget *parent)
    : SshConsole(parent)
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void AlternateConsole::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Backspace:
        emit getData("\x7F");
        isDebug = true;
        break;
    case Qt::Key_Up:
        emit getData("\033OA");
        isDebug = true;
        break;
    case Qt::Key_Down:
        emit getData("\033OB");
        isDebug = true;
        break;
    case Qt::Key_Right:
        emit getData("\033OC");
        isDebug = true;
        break;
    case Qt::Key_Left:
        emit getData("\033OD");
        commandParser->setLeftKeyPress(true);
        isDebug = true;
        break;
    case Qt::Key_Home:
        emit getData("\033OH");
        isDebug = true;
        break;
    case Qt::Key_End:
        emit getData("\033OF");
        isDebug = true;
        break;
    case Qt::Key_Insert:
        emit getData("\033[2~");
        isDebug = true;
        break;
    case Qt::Key_Delete:
        emit getData("\033[3~");
        isDebug = true;
        break;
    case Qt::Key_PageUp:
        emit getData("\033[5~");
        break;
    case Qt::Key_PageDown:
        emit getData("\033[6~");
        break;
    case Qt::Key_F1:
        emit getData("\033OP");
        break;
    case Qt::Key_F2:
        emit getData("\033OQ");
        break;
    case Qt::Key_F3:
        emit getData("\033OR");
        break;
    case Qt::Key_F4:
        emit getData("\033OS");
        break;
    case Qt::Key_F5:
        emit getData("\033[15~");
        break;
    case Qt::Key_F6:
        emit getData("\033[17~");
        break;
    case Qt::Key_F7:
        emit getData("\033[18~");
        break;
    case Qt::Key_F8:
        emit getData("\033[19~");
        break;
    case Qt::Key_F9:
        emit getData("\033[20~");
        break;
    case Qt::Key_F10:
        emit getData("\033[21~");
        isDebug = true;
        break;
    case Qt::Key_F11:
        emit getData("\033[23~");
        break;
    case Qt::Key_F12:
        emit getData("\033[24~");
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        emit getData(e->text().toLocal8Bit());
        break;
    default:
        emit getData(e->text().toLocal8Bit());
        isDebug = true;
        break;
    }
}

void AlternateConsole::mousePressEvent(QMouseEvent *)
{
}

void AlternateConsole::mouseMoveEvent(QMouseEvent *)
{
}

void AlternateConsole::mouseReleaseEvent(QMouseEvent *)
{
}

void AlternateConsole::mouseDoubleClickEvent(QMouseEvent *)
{
}

void AlternateConsole::connectAppCommand()
{
    if(!commandParser)
        return;

    connect(commandParser, SIGNAL(onSwitchToAppKeypadMode()), this, SLOT(onSwitchToAppKeypadMode()));
    connect(commandParser, SIGNAL(onSwitchToNormalKeypadMode()), this, SLOT(onSwitchToNormalKeypadMode()));
    connect(commandParser, SIGNAL(onScreenHome()), this, SLOT(onScreenHome()));
    connect(commandParser, SIGNAL(onASCIIMode()), this, SLOT(onASCIIMode()));
    connect(commandParser, SIGNAL(onDECLineDrawingMode()), this, SLOT(onDECLineDrawingMode()));
    connect(commandParser, SIGNAL(onHideCursor()), this, SLOT(hideCursor()));
    connect(commandParser, SIGNAL(onShowCursor()), this, SLOT(showCursor()));
    connect(commandParser, SIGNAL(onRowRangle(int,int)), this, SLOT(onRowRangle(int,int)));
    connect(commandParser, SIGNAL(onCursorPos(int,int)), this, SLOT(onCursorPos(int,int)));
    connect(commandParser, SIGNAL(onRow(int)), this, SLOT(onRow(int)));
    connect(commandParser, SIGNAL(onCol(int)), this, SLOT(onCol(int)));
    connect(commandParser, SIGNAL(onScrollDown(int)), this, SLOT(scrollDown(int)));
    connect(commandParser, SIGNAL(onScrollUp(int)), this, SLOT(scrollUp(int)));
    connect(commandParser, SIGNAL(onInsertLine(int)), this, SLOT(insertLine(int)));
    connect(commandParser, SIGNAL(onDeleteLine(int)), this, SLOT(deleteLine(int)));
    connect(commandParser, SIGNAL(onUp(int)), this, SLOT(onUp(int)));
    connect(commandParser, SIGNAL(onDelCharToLineHome()), this, SLOT(delCharToLineHome()));
    connect(commandParser, SIGNAL(onEraseChars(int)), this, SLOT(onEraseChars(int)));
    connect(commandParser, SIGNAL(onCleanToScreenEnd()), this, SLOT(onCleanToScreenEnd()));
}

void AlternateConsole::disconnectAppCommand()
{
    if(!commandParser)
        return;

    disconnect(commandParser, SIGNAL(onSwitchToAppKeypadMode()), this, SLOT(onSwitchToAppKeypadMode()));
    disconnect(commandParser, SIGNAL(onSwitchToNormalKeypadMode()), this, SLOT(onSwitchToNormalKeypadMode()));
    disconnect(commandParser, SIGNAL(onScreenHome()), this, SLOT(onScreenHome()));
    disconnect(commandParser, SIGNAL(onASCIIMode()), this, SLOT(onASCIIMode()));
    disconnect(commandParser, SIGNAL(onDECLineDrawingMode()), this, SLOT(onDECLineDrawingMode()));
    disconnect(commandParser, SIGNAL(onHideCursor()), this, SLOT(hideCursor()));
    disconnect(commandParser, SIGNAL(onShowCursor()), this, SLOT(showCursor()));
    disconnect(commandParser, SIGNAL(onRowRangle(int,int)), this, SLOT(onRowRangle(int,int)));
    disconnect(commandParser, SIGNAL(onCursorPos(int,int)), this, SLOT(onCursorPos(int,int)));
    disconnect(commandParser, SIGNAL(onRow(int)), this, SLOT(onRow(int)));
    disconnect(commandParser, SIGNAL(onCol(int)), this, SLOT(onCol(int)));
    disconnect(commandParser, SIGNAL(onScrollDown(int)), this, SLOT(scrollDown(int)));
    disconnect(commandParser, SIGNAL(onScrollUp(int)), this, SLOT(scrollUp(int)));
    disconnect(commandParser, SIGNAL(onInsertLine(int)), this, SLOT(insertLine(int)));
    disconnect(commandParser, SIGNAL(onDeleteLine(int)), this, SLOT(deleteLine(int)));
    disconnect(commandParser, SIGNAL(onUp(int)), this, SLOT(onUp(int)));
    disconnect(commandParser, SIGNAL(onDelCharToLineHome()), this, SLOT(delCharToLineHome()));
    disconnect(commandParser, SIGNAL(onEraseChars(int)), this, SLOT(onEraseChars(int)));
    disconnect(commandParser, SIGNAL(onCleanToScreenEnd()), this, SLOT(onCleanToScreenEnd()));
}

void AlternateConsole::putData(const QByteArray &data)
{
#ifdef SHOW_INFO
    if(isDebug)
        qDebug() << "putData:" << data;
#endif
    commandParser->parse(data);
#ifdef NEW_DRAW
    screen.updateRows(this, palette_, normalFormat);
    onCursorPos(screen.row(), screen.col());
#endif
    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void AlternateConsole::clearScreen()
{
    screen.clearScreen();
}

void AlternateConsole::reset()
{
    //isDebug = false;
    screen.clear(true);
    screen.clearScreen();
    screen.update(this, palette_, normalFormat);
}

void AlternateConsole::shellSize(int cols, int rows)
{
    screen.setSize(cols, rows);
    document()->setMaximumBlockCount(rows);
}

void AlternateConsole::updateRows()
{
#ifdef NEW_DRAW
    if(screen.isCanUpdate())
    {
        screen.updateRows(this, palette_, normalFormat);
        onCursorPos(screen.row(), screen.col());
        QScrollBar *bar = verticalScrollBar();
        bar->setValue(bar->maximum());
    }
    #endif
}

void AlternateConsole::putText(QString const& text)
{
#ifdef SHOW_INFO
    if(isDebug)
        qDebug() << "putText(" << screen.row() << "," << screen.col() << "): " << text;
#endif
    screen.setText(text);
#ifndef NEW_DRAW
    int oldPos = textCursor().position();
    screen.updateRows(this, palette_, normalFormat);
    int newPos = textCursor().position();
    if(newPos != oldPos)
        textCursor().setPosition(oldPos);
#endif
}

void AlternateConsole::setForeColor(ColorRole role)
{
    screen.setReverse(false);
    screen.setForeColor(role);
}

void AlternateConsole::setBackColor(ColorRole role)
{
    screen.setReverse(false);
    screen.setBackColor(role);
}

void AlternateConsole::setReverse()
{
    screen.setReverse(true);
}

void AlternateConsole::setBold(bool enable)
{
    screen.setBold(enable);
}

void AlternateConsole::setUnderLine(bool enable)
{
    screen.setUnderLine(enable);
}

void  AlternateConsole::setCloseColor()
{
    screen.setReverse(false);
    screen.setBold(false);
    screen.setUnderLine(false);
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

void AlternateConsole::cursorRight(int count)
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, count);
    setTextCursor(cursor);
    screen.cursorRight(count);
}

void AlternateConsole::onSwitchToAppKeypadMode()
{
    qDebug() << "onSwitchToAppKeypadMode";
    QTextCharFormat format = textCursor().charFormat();
    cursorWidth =  QFontMetrics(QFont(format.fontFamily(),
                                format.fontPointSize(),
                                format.fontWeight())).horizontalAdvance("W") + 1;
    setCursorWidth(cursorWidth);
}

void AlternateConsole::onSwitchToNormalKeypadMode()
{
    qDebug() << "onSwitchToCmdKeypadMode";
}

void AlternateConsole::onASCIIMode()
{
    qDebug() << "onASCIIMode";
    screen.setDrawLineMode(false);
}

void AlternateConsole::onDECLineDrawingMode()
{
    qDebug() << "onDECLineDrawingMode";
    screen.setDrawLineMode(true);
}

void AlternateConsole::onRowRangle(int top, int bottom)
{
    topRow = top;
    bottomRow = bottom;
    screen.scrollRangle(top, bottom);
}
void AlternateConsole::hideCursor()
{
#ifdef SHOW_INFO
    qDebug() << "hideCursor";
#endif
    setCursorWidth(0);
}

void AlternateConsole::showCursor()
{
#ifdef SHOW_INFO
    qDebug() << "showCursor(" << screen.row() << "," << screen.col() << ")";
#endif
    setCursorWidth(cursorWidth);
    onCursorPos(screen.row(), screen.col());
}

void AlternateConsole::onScreenHome()
{
    onCursorPos(1, 1);
}

void AlternateConsole::onCursorPos(int row, int col)
{
#ifdef SHOW_INFO
    if(isDebug)
        qDebug() << "pos:" << row << "," << col;
#endif
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, row - 1);
    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, col - 1);
    setTextCursor(cursor);
    screen.cursorPos(row, col);
}

void AlternateConsole::onRow(int row)
{
#ifdef SHOW_INFO
    if(isDebug)
        qDebug() << "row:" << row;
#endif
    screen.cursorRow(row);
}

void AlternateConsole::onCol(int col)
{
#ifdef SHOW_INFO
    if(isDebug)
        qDebug() << "col:" << col;
#endif
     screen.cursorCol(col);
}

void AlternateConsole::onUp(int line)
{
    screen.cursorUp(line);
}

void AlternateConsole::scrollDown(int rows)
{
#ifdef SHOW_INFO
    if(isDebug)
        qDebug() << "scrollDown:" << rows;
#endif
    screen.scrollDown(rows);
}

void AlternateConsole::scrollUp(int rows)
{
#ifdef SHOW_INFO
    if(isDebug)
        qDebug() << "scrollUp:" << rows;
#endif
    screen.scrollUp(rows);
}

void AlternateConsole::insertLine(int lines)
{
    screen.insertLine(lines);
}

void AlternateConsole::deleteLine(int lines)
{
    screen.deleteLine(lines);
}

void AlternateConsole::delCharToLineEnd()
{
    screen.delCharToLineEnd();
#ifndef NEW_DRAW
    screen.updateRows(this, palette_, normalFormat);
#endif
}

void AlternateConsole::delCharToLineHome()
{
    screen.delCharToLineHome();
#ifndef NEW_DRAW
    screen.updateRows(this, palette_, normalFormat);
#endif
}

void AlternateConsole::onEraseChars(int count)
{
    screen.eraseChars(count);
#ifndef NEW_DRAW
    screen.updateRows(this, palette_, normalFormat);
#endif
}

void AlternateConsole::onCleanToScreenEnd()
{
    screen.onCleanToScreenEnd();
}

void AlternateConsole::home()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfLine);
    setTextCursor(cursor);
    screen.cursorCol(1);
}

void AlternateConsole::backspace(int count)
{
    screen.cursorLeft(count);
}

void AlternateConsole::delChars(int count)
{
    screen.delChars(count);
#ifndef NEW_DRAW
    screen.updateRows(this, palette_, normalFormat);
#endif
}
