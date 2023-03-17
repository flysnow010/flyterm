#include "sshconsole.h"
#include "core/consoleparser.h"
#include "highlighter/hightlightermanager.h"

#include <QScrollBar>
#include <QKeyEvent>
#include <QFileDialog>
#include <QTextDocumentWriter>
#include <QFontDatabase>
#include <QApplication>
#include <QTextBlock>
#include <QMimeData>
#include <QDebug>

SshConsole::SshConsole(QWidget *parent)
    : QTextEdit(parent)
    , currentForeRole(ColorRole::NullRole)
    , currentBackRole(ColorRole::NullRole)
{
    document()->setMaximumBlockCount(100);
    setContextMenuPolicy(Qt::CustomContextMenu);

    standardSizes = QFontDatabase::standardSizes();

    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::white);
    setPalette(p);

    textFormat.setFontFamily(fontName_);
    textFormat.setFontPointSize(fontSize_);
}

SshConsole::~SshConsole()
{
}

void SshConsole::setCommandParser(ConsoleParser* parser)
{
    commandParser = parser;
}

void SshConsole::connectCommand()
{
    if(!commandParser)
        return;
    connect(commandParser, SIGNAL(onBeep()), this, SLOT(onBeep()));
    connect(commandParser, SIGNAL(onBackspace(int)), this, SLOT(onBackspace(int)));
    connect(commandParser, SIGNAL(onLeft(int)), this, SLOT(onLeft(int)));
    connect(commandParser, SIGNAL(onRight(int)), this, SLOT(onRight(int)));
    connect(commandParser, SIGNAL(onText(QString)), this, SLOT(onText(QString)));
    connect(commandParser, SIGNAL(onForeColor(ColorRole)),
            this, SLOT(onForeColor(ColorRole)));
    connect(commandParser, SIGNAL(onCleanScreen()), this, SLOT(onCleanScreen()));
    connect(commandParser, SIGNAL(onColorClose()), this, SLOT(onColorClose()));
    connect(commandParser, SIGNAL(onSwitchToAlternateScreen()), this, SIGNAL(onSwitchToAlternateScreen()));
    connect(commandParser, SIGNAL(onSwitchToMainScreen()), this, SIGNAL(onSwitchToMainScreen()));
}

void SshConsole::disconnectCommand()
{
    if(!commandParser)
        return;
    disconnect(commandParser, SIGNAL(onBeep()), this, SLOT(onBeep()));
    disconnect(commandParser, SIGNAL(onBackspace(int)), this, SLOT(onBackspace(int)));
    disconnect(commandParser, SIGNAL(onLeft(int)), this, SLOT(onLeft(int)));
    disconnect(commandParser, SIGNAL(onRight(int)), this, SLOT(onRight(int)));
    disconnect(commandParser, SIGNAL(onText(QString)), this, SLOT(onText(QString)));
    disconnect(commandParser, SIGNAL(onForeColor(ColorRole)),
            this, SLOT(onForeColor(ColorRole)));
    disconnect(commandParser, SIGNAL(onCleanScreen()), this, SLOT(onCleanScreen()));
    disconnect(commandParser, SIGNAL(onColorClose()), this, SLOT(onColorClose()));
    disconnect(commandParser, SIGNAL(onSwitchToAlternateScreen()), this, SIGNAL(onSwitchToAlternateScreen()));
    disconnect(commandParser, SIGNAL(onSwitchToMainScreen()), this, SIGNAL(onSwitchToMainScreen()));
}

void SshConsole::setFontName(QString const& fontName)
{
    fontName_ = fontName;
    textFormat.setFontFamily(fontName_);
    normalFormat.setFontFamily(fontName_);
    selectAll();
    setFontFamily(fontName_);
    clearSelection();
}

void SshConsole::setFontSize(int fontSize)
{
    fontSize_ = fontSize;
    textFormat.setFontPointSize(fontSize_);
    normalFormat.setFontPointSize(fontSize_);
    selectAll();
    setFontPointSize(fontSize_);
    clearSelection();
}

void SshConsole::setConsoleColor(ConsoleColor const& color)
{
    QPalette p = palette();
    p.setColor(QPalette::Base, color.back);
    p.setColor(QPalette::Text, color.fore);
    setPalette(p);
}

void SshConsole::setConsolePalette(ConsolePalette::Ptr palette)
{
    palette_ = palette;
    int oldPos = -1;
    for(int i = 0; i < colorRanges.size(); i++)
    {
        int pos = selectText(colorRanges[i].start, colorRanges[i].end);
        setTextColor(palette_->color(colorRanges[i].role).fore);
        if(oldPos == -1)
            oldPos = pos;
    }
    if(oldPos > 0)
    {
        QTextCursor cursor = textCursor();
        cursor.setPosition(oldPos);
        setTextCursor(cursor);
    }
}

void SshConsole::updateColors()
{
    int oldPos = -1;
    for(int i = 0; i < colorRanges.size(); i++)
    {
        if(!(selectStart > colorRanges[i].end || selectEnd < colorRanges[i].start))
        {
            int pos = selectText(colorRanges[i].start, colorRanges[i].end);
            setTextColor(palette_->color(colorRanges[i].role).fore);
            if(oldPos == -1)
                oldPos = pos;
        }
    }
    if(oldPos > 0)
    {
        QTextCursor cursor = textCursor();
        cursor.setPosition(oldPos);
        setTextCursor(cursor);
    }
}

void SshConsole::updateHightLighter(QString const& hightLighter)
{
    if(highlighter)
    {
        highlighter->setDocument(nullptr);
        delete highlighter;
        highlighter = nullptr;
    }
    highlighter = highLighterManager->create(hightLighter);
    if(highlighter)
    {
        highlighter->setDocument(document());
        highlighter->rehighlight();
    }
}

void SshConsole::putData(const QByteArray &data)
{
    commandParser->parse(data);
    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void SshConsole::onBeep()
{
    QApplication::beep();
}

void SshConsole::onBackspace(int count)
{
    QTextCursor cursor = textCursor();
    int pos = cursor.position();
    if(pos - count < minCursorPos_)
        count = pos - minCursorPos_;
    for(int i = 0; i < count; i++)
        textCursor().deletePreviousChar();
}

void SshConsole::onLeft(int count)
{
    cursorLeft(count);
}

void SshConsole::cursorLeft(int count)
{
    QTextCursor cursor = textCursor();
    int pos = cursor.position();
    if(pos - count < minCursorPos_)
        count = pos - minCursorPos_;
    cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, count);
    setTextCursor(cursor);
}

void SshConsole::onRight(int count)
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, count);
    setTextCursor(cursor);
}

void SshConsole::onReturn()
{
    int pos = textCursor().position();
    while(pos > minCursorPos_ && minCursorPos_ > 0)
    {
        textCursor().deletePreviousChar();
        pos = textCursor().position();
    }
}

void SshConsole::onEnd()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    setTextCursor(cursor);
}

void SshConsole::onText(QString const& text)
{
    if(logfile_)
        logfile_->write(text);
    putText(text);
}

void SshConsole::putText(QString const& text)
{
    if(text == "\r")
    {
        onReturn();
        return;
    }

    QTextCursor tc = textCursor();
    tc.insertText(text, isUseColor ? textFormat : normalFormat);
    if(isUseColor)
    {
        ColorRange colorRange;
        colorRange.role = currentForeRole;
        colorRange.end = tc.position();
        colorRange.start = colorRange.end - text.size();
        colorRanges << colorRange;
    }
}

void SshConsole::clearScreen()
{

}

void SshConsole::onForeColor(ColorRole role)
{
    setForeColor(role);
}

void SshConsole::setForeColor(ColorRole role)
{
    isUseColor = true;
    currentForeRole = role;
    if(palette_)
        textFormat.setForeground(QBrush(palette_->color(role).fore));
}

void SshConsole::onBackColor(ColorRole role)
{
    setBackColor(role);
}

void SshConsole::setBackColor(ColorRole role)
{
    isUseColor = true;
    currentBackRole = role;
    if(palette_)
        textFormat.setBackground(QBrush(palette_->color(role).back));
}

void SshConsole::setCloseColor()
{
    isUseColor = false;
    currentBackRole = ColorRole::NullRole;
    currentForeRole = ColorRole::NullRole;
    textFormat.setForeground(palette().color(QPalette::Text));
}
void SshConsole::onCleanScreen()
{
    clear();
}

void SshConsole::onColorClose()
{
    setCloseColor();
}

void SshConsole::keyPressEvent(QKeyEvent *e)
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
    case Qt::Key_Home:
        emit getData("\033[H");
        break;
    case Qt::Key_End:
        emit getData("\033[F");
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        onEnd();
        emit getData(e->text().toLocal8Bit());
        break;
    default:
        emit getData(e->text().toLocal8Bit());
        break;
    }
    int pos = textCursor().position();
    if(minCursorPos_ < 0 || pos < minCursorPos_)
        minCursorPos_ = pos;
}

void SshConsole::mousePressEvent(QMouseEvent *e)
{
    cursorPos = textCursor().position();
    if(e->buttons() & Qt::LeftButton)
        cancelSelection();

    QTextEdit::mousePressEvent(e);
}

void SshConsole::mouseReleaseEvent(QMouseEvent *e)
{
    QTextEdit::mouseReleaseEvent(e);

    QTextCursor cursor = textCursor();
    setTextColor(palette().color(QPalette::HighlightedText));
    setTextBackgroundColor(palette().color(QPalette::Highlight));
    if(cursor.hasSelection())
    {
        selectStart = cursor.selectionStart();
        selectEnd = cursor.selectionEnd();
    }

    QScrollBar *bar = verticalScrollBar();
    int value = bar->value();

    cursor.setPosition(cursorPos);
    setTextCursor(cursor);

    bar->setValue(value);
}

void SshConsole::mouseDoubleClickEvent(QMouseEvent *e)
{
    int pos = textCursor().position();
    QTextEdit::mouseDoubleClickEvent(e);
    setTextColor(palette().color(QPalette::HighlightedText));
    setTextBackgroundColor(palette().color(QPalette::Highlight));

    QTextCursor cursor = textCursor();
    selectStart = cursor.selectionStart();
    selectEnd = cursor.selectionEnd();
    cursor.setPosition(pos);
    setTextCursor(cursor);
}

void SshConsole::insertFromMimeData(const QMimeData *source)
{
    if(source->hasText())
       emit getData(source->text().toUtf8());
    QTextEdit::insertFromMimeData(source);
}

void SshConsole::copyAll()
{
    selectAll();
    copy();
    clearSelection();
}

void SshConsole::clearall()
{
    clear();
    colorRanges.clear();
}

void SshConsole::copyOne()
{
    if(selectStart != selectEnd)
    {
        int pos = selectText();
        setTextColor(palette().color(QPalette::Text));
        setTextBackgroundColor(palette().color(QPalette::Base));
        copy();

        QTextCursor cursor = textCursor();
        cursor.setPosition(pos);
        setTextCursor(cursor);
    }
}

bool SshConsole::increaseFontSize()
{
    int fontIndex = standardSizes.indexOf(fontSize_) + 1;
    if(fontIndex < standardSizes.size())
    {
        setFontSize(standardSizes.at(fontIndex));
        return  true;
    }
    return false;
}

bool SshConsole::decreaseFontSize()
{
    int fontIndex = standardSizes.indexOf(fontSize_) - 1;
    if(fontIndex >= 0)
    {
        setFontSize(standardSizes.at(fontIndex));
        return true;
    }
    return false;
}

void SshConsole::saveToFile()
{
    QFileDialog fileDialog(this, tr("Save as..."));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    QStringList mimeTypes;
    mimeTypes  << "text/plain" << "text/html";
    fileDialog.setMimeTypeFilters(mimeTypes);
    fileDialog.setDefaultSuffix("txt");
    if (fileDialog.exec() != QDialog::Accepted)
        return;

   QString fileName = fileDialog.selectedFiles().first();
   if(fileName.isEmpty())
       return;

   QTextDocumentWriter writer(fileName);
   writer.write(document());
}

void SshConsole::clearSelection()
{
    QTextCursor cursor = textCursor();
    cursor.clearSelection();
    setTextCursor(cursor);  
}

void SshConsole::cancelSelection()
{
    if(selectStart != selectEnd)
    {
        int pos = selectText();
        setTextColor(palette().color(QPalette::Text));
        setTextBackgroundColor(palette().color(QPalette::Base));
        QTextCursor cursor = textCursor();
        cursor.setPosition(pos);
        setTextCursor(cursor);
        updateColors();
        selectStart = 0;
        selectEnd = 0;
    }
}

int SshConsole::selectText()
{
    return selectText(selectStart, selectEnd);
}

int SshConsole::selectText(int start, int end)
{
    QTextCursor cursor = textCursor();
    int pos = cursor.position();
    cursor.setPosition(start);
    cursor.movePosition(QTextCursor::Right,
                        QTextCursor::KeepAnchor, end - start);
    setTextCursor(cursor);
    return pos;
}
