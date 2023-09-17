#include "console.h"
#include "core/consoleparser.h"
#include "highlighter/hightlightermanager.h"
#include <QScrollBar>
#include <QMimeData>
#include <QApplication>
#include <QFileDialog>
#include <QTextDocumentWriter>

Console::Console(QWidget *parent)
    : QPlainTextEdit(parent)
{
    document()->setMaximumBlockCount(0);
    setContextMenuPolicy(Qt::CustomContextMenu);
    standardSizes = QFontDatabase::standardSizes();

    textFormat.setFontFamily(fontName_);
    textFormat.setFontPointSize(fontSize_);
    normalFormat.setFontFamily(fontName_);
    normalFormat.setFontPointSize(fontSize_);
}

Console::~Console()
{
    delete commandParser;
    delete highlighter;
}

void Console::connectCommands()
{
    if(!commandParser)
        return;

    connect(commandParser, SIGNAL(onBeep()), this, SLOT(onBeep()));
    connect(commandParser, SIGNAL(onGetCursorPos()), this, SLOT(onGetCursorPos()));
    connect(commandParser, SIGNAL(onForeColor(ColorRole)),
            this, SLOT(onForeColor(ColorRole)));
    connect(commandParser, SIGNAL(onCloseCharAttriutes()), this, SLOT(onCloseCharAttriutes()));
    connect(commandParser, SIGNAL(onBackspace(int)), this, SLOT(onBackspace(int)));
    connect(commandParser, SIGNAL(onLeft(int)), this, SLOT(onLeft(int)));
    connect(commandParser, SIGNAL(onRight(int)), this, SLOT(onRight(int)));
    connect(commandParser, SIGNAL(onText(QString)), this, SLOT(onText(QString)));
}

void Console::disconnectCommands()
{
    if(!commandParser)
        return;

    disconnect(commandParser, SIGNAL(onBeep()), this, SLOT(onBeep()));
    disconnect(commandParser, SIGNAL(onGetCursorPos()), this, SLOT(onGetCursorPos()));
    disconnect(commandParser, SIGNAL(onForeColor(ColorRole)),
            this, SLOT(onForeColor(ColorRole)));
    disconnect(commandParser, SIGNAL(onCloseCharAttriutes()), this, SLOT(onCloseCharAttriutes()));
    disconnect(commandParser, SIGNAL(onBackspace(int)), this, SLOT(onBackspace(int)));
    disconnect(commandParser, SIGNAL(onLeft(int)), this, SLOT(onLeft(int)));
    disconnect(commandParser, SIGNAL(onRight(int)), this, SLOT(onRight(int)));
    disconnect(commandParser, SIGNAL(onText(QString)), this, SLOT(onText(QString)));
}

void Console::copyAll()
{
    selectAll();
    copy();
    clearSelection();
}

void Console::clearall()
{
    clear();
    colorRanges.clear();
}

void Console::copyOne()
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

bool Console::increaseFontSize()
{
    int fontIndex = standardSizes.indexOf(fontSize_) + 1;
    if(fontIndex < standardSizes.size())
    {
        setFontSize(standardSizes.at(fontIndex));
        return true;
    }
    return false;
}

bool Console::decreaseFontSize()
{
    int fontIndex = standardSizes.indexOf(fontSize_) - 1;
    if(fontIndex >= 0)
    {
        setFontSize(standardSizes.at(fontIndex));
        return true;
    }
    return false;
}

void Console::saveToFile()
{
    QFileDialog fileDialog(this, tr("Save as..."));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    QStringList mimeTypes;
    mimeTypes << "text/plain" << "text/html";
    fileDialog.setMimeTypeFilters(mimeTypes);
    fileDialog.setDefaultSuffix("txt");
    if (fileDialog.exec() != QDialog::Accepted)
        return;

   QStringList fileNames = fileDialog.selectedFiles();
   if(fileNames.isEmpty())
       return;
   QTextDocumentWriter writer(fileNames[0]);
   writer.write(document());
}

void Console::putData(const QByteArray &data)
{
    commandParser->parse(data);
    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Console::setCodecName(QString const& name)
{
    commandParser->setCodecName(name.toUtf8());
}

void Console::setFontName(QString const& name)
{
    fontName_ = name;
    textFormat.setFontFamily(fontName_);
    normalFormat.setFontFamily(fontName_);
    selectAll();
    setFontFamily(fontName_);
    clearSelection();
}

void Console::setFontSize(int fontSize)
{
    fontSize_ = fontSize;
    textFormat.setFontPointSize(fontSize_);
    normalFormat.setFontPointSize(fontSize_);
    selectAll();
    setFontPointSize(fontSize_);
    clearSelection();
}

void Console::setConsoleColor(ConsoleColor const& color)
{
    setStyleSheet(QString("QPlainTextEdit { color: %1; background: %2; }")
                  .arg(color.fore.name(), color.back.name()));
}

void Console::setConsolePalette(ConsolePalette::Ptr palette)
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

void Console::updateHightLighter(QString const& hightLighter)
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

void Console::cancelSelection()
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

void Console::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Backspace:
        emit getData(e->text().toLocal8Bit());
        break;
    case Qt::Key_Left:
        emit getData("\033[D");
        commandParser->setLeftKeyPress(true);
        break;
    case Qt::Key_Right:
        emit getData("\033[C");
        commandParser->setRightKeyPress(true);
        break;
    case Qt::Key_Up:
        emit getData("\033[A");
        break;
    case Qt::Key_Down:
        emit getData("\033[B");
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        onEnd();
        emit getData(e->text().toLocal8Bit());

        break;
    default:
        if (isLocalEchoEnabled())
            QPlainTextEdit::keyPressEvent(e);
        emit getData(e->text().toLocal8Bit());
        break;
    }
}

void Console::mousePressEvent(QMouseEvent * e)
{
    cursorPos = textCursor().position();
    if(e->buttons() & Qt::LeftButton)
        cancelSelection();

    QPlainTextEdit::mousePressEvent(e);
}

void Console::mouseReleaseEvent(QMouseEvent *e)
{
    QPlainTextEdit::mouseReleaseEvent(e);

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

void Console::mouseDoubleClickEvent(QMouseEvent *e)
{
    int pos = textCursor().position();
    QPlainTextEdit::mouseDoubleClickEvent(e);
    setTextColor(palette().color(QPalette::HighlightedText));
    setTextBackgroundColor(palette().color(QPalette::Highlight));

    QTextCursor cursor = textCursor();
    selectStart = cursor.selectionStart();
    selectEnd = cursor.selectionEnd();
    cursor.setPosition(pos);
    setTextCursor(cursor);
}

void Console::insertFromMimeData(const QMimeData *source)
{
    if(source->hasText())
       emit getData(source->text().toUtf8());
}

ConsoleParser * Console::createParser()
{
    return new ConsoleParser();
}

void Console::onBeep()
{
    QApplication::beep();
}

void Console::onGetCursorPos()
{
    emit onGotCursorPos(screen.row(), screen.col());
}

void Console::onBackspace(int count)
{
    QTextCursor cursor = textCursor();
    for(int i = 0; i < count; i++)
        cursor.deletePreviousChar();
    setTextCursor(cursor);
}

void Console::onLeft(int count)
{
    QTextCursor cursor = textCursor();
    int pos = cursor.positionInBlock();
    if(count < pos)
        cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, count);
    setTextCursor(cursor);
}

void Console::onRight(int count)
{
    QTextCursor cursor = textCursor();
    if(count + screen.col() >= screen.cols())
        count = screen.cols() - screen.col();

    screen.cursorRight(count);
    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, count);

}

void Console::onText(QString const& text)
{
    LogFile::SharedPtr loginfile = logfile_.lock();
    if(loginfile)
        loginfile->write(text);
    putText(text);
}

void Console::putText(QString const& text)
{
    QTextCursor tc = textCursor();
    int start = tc.position();
    QTextCharFormat* format = isUseColor ? &textFormat : &normalFormat;
    int index = -1;
    for(int i = 0; i < text.size(); i++)
    {
        if(text[i] == '\r')
        {
            if(index >= 0)
            {
                tc.insertText(text.mid(index, i - index), *format);
                index = -1;
            }
            isReturn = true;
        }
        else if(text[i] == '\n')
        {
            if(isReturn)
            {
                isReturn = false;
                tc.insertText(text[i], *format);
            }
            else if(index >= 0)
            {
                tc.insertText(text.mid(index, i - index + 1), *format);
                index = -1;
            }
        }
        else
        {
            if(index < 0)
                index = i;
            if(isReturn)
            {
                removeCurrentRow();
                if(tc.position() < start)
                    start = tc.position();
                isReturn = false;
            }
            if(i == text.size() - 1)
                tc.insertText(text.mid(index, i - index + 1), *format);
        }
    }
    if(isUseColor)
    {
        ColorRange colorRange;
        colorRange.role = currentForeRole;
        colorRange.end = tc.position();
        colorRange.start = start;
        colorRanges << colorRange;
    }
}

void Console::onForeColor(ColorRole role)
{
    setForeColor(role);
}

void Console::onBackColor(ColorRole role)
{
    setBackColor(role);
}

void Console::onCloseCharAttriutes()
{
    setCloseColor();
}

void Console::setCloseColor()
{
    isUseColor = false;
    currentBackRole = ColorRole::NullRole;
    currentForeRole = ColorRole::NullRole;
    textFormat.setForeground(palette().color(QPalette::Text));
    textFormat.setBackground(palette().color(QPalette::Base));
}

void Console::onEnd()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    setTextCursor(cursor);
}

void Console::createParserAndConnect()
{
    commandParser = createParser();
    connectCommands();
}

void Console::removeCurrentRow()
{
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::LineUnderCursor);
    cursor.removeSelectedText();
    setTextCursor(cursor);
}

void Console::clearSelection()
{
    QTextCursor cursor = textCursor();
    cursor.clearSelection();
    setTextCursor(cursor);
}

int Console::selectText()
{
    return selectText(selectStart, selectEnd);
}

int Console::selectText(int start, int end)
{
    QTextCursor cursor = textCursor();
    int pos = cursor.position();
    cursor.setPosition(start);
    cursor.movePosition(QTextCursor::Right,
                        QTextCursor::KeepAnchor, end - start);
    setTextCursor(cursor);
    return pos;
}

void Console::updateColors()
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

void Console::setForeColor(ColorRole role)
{
    isUseColor = true;
    currentForeRole = role;
    if(palette_)
        textFormat.setForeground(QBrush(palette_->color(role).fore));
}

void Console::setBackColor(ColorRole role)
{
    isUseColor = true;
    currentBackRole = role;
    if(palette_)
        textFormat.setBackground(QBrush(palette_->color(role).back));
}

void Console::setTextColor(QColor const& color)
{
    QTextCursor tc = textCursor();
    QTextCharFormat format;
    format.setForeground(color);
    tc.mergeCharFormat(format);
}
void Console::setTextBackgroundColor(QColor const& color)
{
    QTextCursor tc = textCursor();
    QTextCharFormat format;
    format.setBackground(color);
    tc.mergeCharFormat(format);
}

void Console::setFontFamily(QString const& name)
{
    QTextCursor tc = textCursor();
    QTextCharFormat format;
    format.setFontFamily(name);
    tc.mergeCharFormat(format);
}

void Console::setFontPointSize(int fontSize)
{
    QTextCursor tc = textCursor();
    QTextCharFormat format;
    format.setFontPointSize(fontSize);
    tc.mergeCharFormat(format);
}
