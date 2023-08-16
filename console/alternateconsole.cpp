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

AlternateConsole::AlternateConsole(QWidget *parent)
    : SshConsole(parent)
{
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
        isUpdate = true;
        break;
    case Qt::Key_Down:
        emit getData("\033OB"); //"\x1B[?25l\x1B[1;23r\x1B[23;1H\r\n\x1B[1;24r\x1B[23;1H#endif\x1B[24;1H\x1B[K\x1B[23;1H\x1B[?12l\x1B[?25h"
        isUpdate = true;
        break;
    case Qt::Key_Right:
        emit getData("\033OC");
        isUpdate = true;
        break;
    case Qt::Key_Left:
        emit getData("\033OD");
        commandParser->setLeftKeyPress(true);
        isUpdate = true;
        break;
    case Qt::Key_Home:
        emit getData("\033OH");
        isUpdate = true;
        break;
    case Qt::Key_End:
        emit getData("\033OF");
        isUpdate = true;
        break;
    case Qt::Key_Insert:
        emit getData("\033[2~");
        isUpdate = true;
        break;
    case Qt::Key_Delete:
        emit getData("\033[3~");
        isUpdate = true;
        break;
    case Qt::Key_PageUp:
        emit getData("\033[5~");
        //isUpdate = true;
        break;
    case Qt::Key_PageDown:
        emit getData("\033[6~");
        //isUpdate = true;
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        emit getData(e->text().toLocal8Bit());//"\x1B[?25l\x1B[16;23r\x1B[16;1H\x1B[L\x1B[1;24r\x1B[16;1H\x1B[?12l\x1B[?25h"
        break;
    default:
        emit getData(e->text().toLocal8Bit());
        isUpdate = true;
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
    disconnect(commandParser, SIGNAL(onUp(int)), this, SLOT(onUp(int)));
    disconnect(commandParser, SIGNAL(onDelCharToLineHome()), this, SLOT(delCharToLineHome()));
    disconnect(commandParser, SIGNAL(onEraseChars(int)), this, SLOT(onEraseChars(int)));
    disconnect(commandParser, SIGNAL(onCleanToScreenEnd()), this, SLOT(onCleanToScreenEnd()));
}

void AlternateConsole::putData(const QByteArray &data)
{
#ifdef SHOW_INFO
    if(isUpdate)
        qDebug() << "putData:" << data;
#endif
    commandParser->parse(data);
    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void AlternateConsole::clearScreen()
{
    //screen.clear(false);
    screen.clearScreen();
}

void AlternateConsole::reset(bool video)
{
    isVideo = video;
    isUpdate = false;
    screen.clear(true);
//    if(!isVideo)
//        screen.clear(true);
//    else
    if(isVideo)
    {
        screen.clearScreen();
        screen.update(this, palette_, normalFormat);
    }
}

void AlternateConsole::shellSize(int cols, int rows)
{
    screen.setSize(cols, rows);
    document()->setMaximumBlockCount(rows);
}

void AlternateConsole::putText(QString const& text)
{
#ifdef SHOW_INFO
    if(isUpdate)
        qDebug() << "putText(" << screen.row() << "," << screen.col() << "): " << text;
#endif
    if(text == "\r")
    {
        if(!isPutText || isUpdate)
            home();
        return;
    }
    if(text == "\r\n")
    {
        if(!isPutText)
        {
            down();
            return;
        }
        else if(screen.isBottom() && isUpdate)
        {
            screen.scrollUp(1);
            return;
        }
    }
    if(isPutText)
    {
        screen.setText(text);
        if(isVideo)
            screen.updateRows(this, palette_, normalFormat);
    }
    else
    {
        onRight(text.size());
        screen.cursorRight(text.size());
    }
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
    int w =  QFontMetrics(QFont(fontFamily(),
                                fontPointSize(),
                                fontWeight())).horizontalAdvance("W");
    setCursorWidth(w + 1);
}

void AlternateConsole::onSwitchToNormalKeypadMode()
{
    qDebug() << "onSwitchToCmdKeypadMode";
}

void AlternateConsole::onASCIIMode()
{
    //qDebug() << "onASCIIMode";
    isPutText = true;
    screen.setDrawLineMode(false);
}

void AlternateConsole::onDECLineDrawingMode()
{
    //qDebug() << "onDECLineDrawingMode";
    isPutText = true;
    screen.setDrawLineMode(true);
}

void AlternateConsole::onRowRangle(int top, int bottom)
{
    //qDebug() << "top:" << top << "bottom: " << bottom;
    topRow = top;
    bottomRow = bottom;
    screen.scrollRangle(top, bottom);
}
void AlternateConsole::hideCursor()
{
#ifdef SHOW_INFO
    qDebug() << "hideCursor";
#endif
    isPutText = true;
}

void AlternateConsole::showCursor()
{
#ifdef SHOW_INFO
    qDebug() << "showCursor";
#endif
    isPutText = false;
    if(!isUpdate)
        screen.update(this, palette_, normalFormat);
    else
    {
        isUpdate = false;
        screen.updateRows(this, palette_, normalFormat);
    }
    onCursorPos(screen.row(), screen.col());
}

void AlternateConsole::onScreenHome()
{
    onCursorPos(1, 1);
}

void AlternateConsole::onCursorPos(int row, int col)
{
#ifdef SHOW_INFO
    if(isUpdate)
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
    if(isUpdate)
        qDebug() << "row:" << row;
#endif
    screen.cursorRow(row);
}

void AlternateConsole::onCol(int col)
{
#ifdef SHOW_INFO
    if(isUpdate)
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
    if(isUpdate)
        qDebug() << "scrollDown:" << rows;
#endif
    screen.scrollDown(rows);
    isUpdate = false;
}

void AlternateConsole::scrollUp(int rows)
{
#ifdef SHOW_INFO
    if(isUpdate)
        qDebug() << "scrollUp:" << rows;
#endif
    screen.scrollUp(rows);
    isUpdate = false;
}

void AlternateConsole::delCharToLineEnd()
{
    screen.delCharToLineEnd();
    screen.updateRows(this, palette_, normalFormat);
}

void AlternateConsole::delCharToLineHome()
{
    screen.delCharToLineHome();
    screen.updateRows(this, palette_, normalFormat);
}

void AlternateConsole::onEraseChars(int count)
{
    screen.onEraseChars(count);
    screen.updateRows(this, palette_, normalFormat);
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

void AlternateConsole::down()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::Down);
    setTextCursor(cursor);
    screen.cursorDown(1);
}
