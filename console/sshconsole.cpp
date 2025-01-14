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
#include <QInputMethodEvent>
#include <QDebug>

SshConsole::SshConsole(QWidget *parent)
    : QPlainTextEdit(parent)
    , currentForeRole(ColorRole::NullRole)
    , currentBackRole(ColorRole::NullRole)
{
    document()->setMaximumBlockCount(0);
    setContextMenuPolicy(Qt::CustomContextMenu);

    standardSizes = QFontDatabase::standardSizes();

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
    connect(commandParser, SIGNAL(onDelChars(int)), this, SLOT(onDelChars(int)));
    connect(commandParser, SIGNAL(onLeft(int)), this, SLOT(onLeft(int)));
    connect(commandParser, SIGNAL(onRight(int)), this, SLOT(onRight(int)));
    connect(commandParser, SIGNAL(onText(QString)), this, SLOT(onText(QString)));
    connect(commandParser, SIGNAL(onForeColor(ColorRole)),
            this, SLOT(onForeColor(ColorRole)));
    connect(commandParser, SIGNAL(onBackColor(ColorRole)),
            this, SLOT(onBackColor(ColorRole)));
    connect(commandParser, SIGNAL(onCleanScreen()), this, SLOT(onCleanScreen()));
    connect(commandParser, SIGNAL(onCloseCharAttriutes()), this, SLOT(onCloseCharAttriutes()));
    connect(commandParser, SIGNAL(onRestoreCursorPos()), this, SLOT(onRestoreState()));
    connect(commandParser, SIGNAL(onGetCursorPos()), this, SLOT(onGetCursorPos()));
    connect(commandParser, SIGNAL(onHome()), this, SLOT(onHome()));
    connect(commandParser, SIGNAL(onDelCharToLineEnd()), this, SLOT(onDelCharToLineEnd()));
    connect(commandParser, SIGNAL(onOverWrite(bool)), this, SLOT(onOverWrite(bool)));
    connect(commandParser, SIGNAL(onSwitchToAlternateScreen()), this, SLOT(switchToAlternateScreen()));
    connect(commandParser, SIGNAL(onSwitchToAppKeypadMode()), this, SIGNAL(onSwitchToAppKeypadMode()));
    connect(commandParser, SIGNAL(onSwitchToNormalKeypadMode()), this, SIGNAL(onSwitchToNormalKeypadMode()));
    connect(commandParser, SIGNAL(onSwitchToMainScreen()), this, SIGNAL(onSwitchToMainScreen()));
    connect(commandParser, SIGNAL(onSaveCursorPos()), this, SIGNAL(onSaveCursorPos()));
    connect(commandParser, SIGNAL(onRestoreCursorPos()), this, SIGNAL(onRestoreCursorPos()));
    connect(commandParser, SIGNAL(onTitle(QString)), this, SIGNAL(onTitle(QString)));
    connect(commandParser, SIGNAL(onReverse()), this, SLOT(onReverse()));
    connect(commandParser, SIGNAL(onBold(bool)), this, SLOT(onBold(bool)));
    connect(commandParser, SIGNAL(onUnderLine(bool)), this, SLOT(onUnderLine(bool)));
}

void SshConsole::disconnectCommand()
{
    if(!commandParser)
        return;
    disconnect(commandParser, SIGNAL(onBeep()), this, SLOT(onBeep()));
    disconnect(commandParser, SIGNAL(onBackspace(int)), this, SLOT(onBackspace(int)));
    disconnect(commandParser, SIGNAL(onDelChars(int)), this, SLOT(onDelChars(int)));
    disconnect(commandParser, SIGNAL(onLeft(int)), this, SLOT(onLeft(int)));
    disconnect(commandParser, SIGNAL(onRight(int)), this, SLOT(onRight(int)));
    disconnect(commandParser, SIGNAL(onText(QString)), this, SLOT(onText(QString)));
    disconnect(commandParser, SIGNAL(onForeColor(ColorRole)),
            this, SLOT(onForeColor(ColorRole)));
    disconnect(commandParser, SIGNAL(onBackColor(ColorRole)),
            this, SLOT(onBackColor(ColorRole)));
    disconnect(commandParser, SIGNAL(onCleanScreen()), this, SLOT(onCleanScreen()));
    disconnect(commandParser, SIGNAL(onCloseCharAttriutes()), this, SLOT(onCloseCharAttriutes()));
    disconnect(commandParser, SIGNAL(onRestoreCursorPos()), this, SLOT(onRestoreState()));
    disconnect(commandParser, SIGNAL(onGetCursorPos()), this, SLOT(onGetCursorPos()));
    disconnect(commandParser, SIGNAL(onHome()), this, SLOT(onHome()));
    disconnect(commandParser, SIGNAL(onDelCharToLineEnd()), this, SLOT(onDelCharToLineEnd()));
    disconnect(commandParser, SIGNAL(onOverWrite(bool)), this, SLOT(onOverWrite(bool)));
    disconnect(commandParser, SIGNAL(onSwitchToAlternateScreen()), this, SLOT(switchToAlternateScreen()));
    disconnect(commandParser, SIGNAL(onSwitchToAppKeypadMode()), this, SIGNAL(onSwitchToAppKeypadMode()));
    disconnect(commandParser, SIGNAL(onSwitchToNormalKeypadMode()), this, SIGNAL(onSwitchToNormalKeypadMode()));
    disconnect(commandParser, SIGNAL(onSwitchToMainScreen()), this, SIGNAL(onSwitchToMainScreen()));
    disconnect(commandParser, SIGNAL(onSaveCursorPos()), this, SIGNAL(onSaveCursorPos()));
    disconnect(commandParser, SIGNAL(onRestoreCursorPos()), this, SIGNAL(onRestoreCursorPos()));
    disconnect(commandParser, SIGNAL(onTitle(QString)), this, SIGNAL(onTitle(QString)));
    disconnect(commandParser, SIGNAL(onReverse()), this, SLOT(onReverse()));
    disconnect(commandParser, SIGNAL(onBold(bool)), this, SLOT(onBold(bool)));
    disconnect(commandParser, SIGNAL(onUnderLine(bool)), this, SLOT(onUnderLine(bool)));
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
    setStyleSheet(QString("QPlainTextEdit { color: %1; background: %2; }")
                  .arg(color.fore.name(), color.back.name()));
    textFormat.setBackground(palette().color(QPalette::Base));
    selectAll();
    setTextBackgroundColor(palette().color(QPalette::Base));
    clearSelection();
}

void SshConsole::setConsolePalette(ConsolePalette::Ptr palette)
{
    palette_ = palette;
    updateColors();
}

void SshConsole::updateColors()
{
    int oldPos = -1;
    QColor defaultBack = this->palette().color(QPalette::Base);
    for(int i = 0; i < colorRanges.size(); i++)
    {
        int pos = selectText(colorRanges[i].start, colorRanges[i].end);
        if(colorRanges[i].back != ColorRole::NullRole)
            setTextColor(palette_->color(colorRanges[i].fore).fore,
                         palette_->color(colorRanges[i].back).back);
        else
            setTextColor(palette_->color(colorRanges[i].fore).fore, defaultBack);
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

void SshConsole::updateSelectedColors()
{
    int oldPos = -1;
    for(int i = 0; i < colorRanges.size(); i++)
    {
        if(!(selectStart > colorRanges[i].end || selectEnd < colorRanges[i].start))
        {
            int pos = selectText(colorRanges[i].start, colorRanges[i].end);
            setTextColor(palette_->color(colorRanges[i].fore).fore);
            if(colorRanges[i].back != ColorRole::NullRole)
                setTextBackgroundColor(palette_->color(colorRanges[i].back).back);
            else
                setTextBackgroundColor(palette().color(QPalette::Base));
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
    if(isSwitchingToAlternate)
    {
        isSwitchingToAlternate = false;
        emit onSwitchToAlternateFinished();
    }
    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void SshConsole::onBeep()
{
    QApplication::beep();
}

void SshConsole::onGetCursorPos()
{
    emit onGotCursorPos(screen.row(), screen.col());
}

void SshConsole::onBackspace(int count)
{
    backspace(count);
}

void SshConsole::onDelChars(int count)
{
    delChars(count);
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

void SshConsole::cursorRight(int count)
{
    if(isReturn)
    {
        home();
        isReturn = false;
    }
    QTextCursor cursor = textCursor();
    if(count + screen.col() < screen.cols())
    {
        cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, count);
        screen.cursorRight(count);
    }
    else
    {
        screen.cursorRight(screen.cols() - screen.col());
        cursor.movePosition(QTextCursor::Right,
                            QTextCursor::MoveAnchor,
                            screen.cols() - screen.col());
    }
    setTextCursor(cursor);
}

void SshConsole::home()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfLine);
    setTextCursor(cursor);
    screen.cursorCol(1);
}

void SshConsole::backspace(int count)
{
    QTextCursor cursor = textCursor();
    int pos = cursor.position();
    if(pos - count < minCursorPos_)
        count = pos - minCursorPos_;
    for(int i = 0; i < count; i++)
        textCursor().deletePreviousChar();
}

void SshConsole::delChars(int count)
{
    Q_UNUSED(count)
    delCharToLineEnd();
}

void SshConsole::onRight(int count)
{
    cursorRight(count);
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

void SshConsole::onHome()
{
    home();
}

void SshConsole::onOverWrite(bool enable)
{
    isOverWrite = enable;
}

void SshConsole::onEnd()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    setTextCursor(cursor);
}

void SshConsole::onDelCharToLineEnd()
{
    delCharToLineEnd();
}

void SshConsole::delCharToLineEnd()
{
    QTextCursor cursor(textCursor());
    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    setTextCursor(cursor);
    textCursor().deleteChar();
}

void SshConsole::onText(QString const& text)
{
    LogFile::SharedPtr logfile = logfile_.lock();
    if(logfile)
        logfile->write(text);

    putText(text);
}

void SshConsole::putText(QString const& text)
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
        colorRange.fore = currentForeRole;
        colorRange.back = currentBackRole;
        colorRange.end = tc.position();
        colorRange.start = start;
        colorRanges << colorRange;
    }
}

void SshConsole::clearScreen()
{
    clear();
}

void SshConsole::onForeColor(ColorRole role)
{
    setForeColor(role);
}

void SshConsole::setForeColor(ColorRole role)
{
    currentForeRole = role;
    if(currentForeRole == ColorRole::NullRole)
    {
        isUseColor = false;
        textFormat.setForeground(palette().color(QPalette::Text));
    }
    else
    {
        isUseColor = true;
        if(palette_)
            textFormat.setForeground(QBrush(palette_->color(role).fore));
    }
}

void SshConsole::onBackColor(ColorRole role)
{
    setBackColor(role);
}

void SshConsole::onReverse()
{
    setReverse();
}

void SshConsole::onBold(bool enable)
{
    setBold(enable);
}

void SshConsole::onUnderLine(bool enable)
{
    setUnderLine(enable);
}

void SshConsole::setReverse()
{
    ColorRole back = currentForeRole;
    ColorRole fore = currentBackRole;
    if(back == ColorRole::NullRole)
        textFormat.setBackground(palette().color(QPalette::Text));
    else
        setBackColor(back);

    if(fore == ColorRole::NullRole)
        textFormat.setForeground(palette().color(QPalette::Base));
    else
        setForeColor(fore);
}

void SshConsole::setBold(bool enable)
{
    textFormat.setFontWeight(enable ? QFont::Bold : QFont::Normal);
}

void SshConsole::setUnderLine(bool enable)
{
    textFormat.setFontUnderline(enable);
}

void SshConsole::setBackColor(ColorRole role)
{
    currentBackRole = role;
    if(currentBackRole == ColorRole::NullRole)
    {
        isUseColor = false;
        textFormat.setBackground(palette().color(QPalette::Base));
    }
    else
    {
        isUseColor = true;
        if(palette_)
            textFormat.setBackground(QBrush(palette_->color(role).back));
    }
}

void SshConsole::setCloseColor()
{
    isUseColor = false;
    setBold(false);
    setUnderLine(false);
    currentBackRole = ColorRole::NullRole;
    currentForeRole = ColorRole::NullRole;
    textFormat.setForeground(palette().color(QPalette::Text));
    textFormat.setBackground(palette().color(QPalette::Base));
}
void SshConsole::onCleanScreen()
{
    clearScreen();
}

void SshConsole::onCloseCharAttriutes()
{
    setCloseColor();
}

void SshConsole::onRestoreState()
{
    setCloseColor();
    isReturn = true;
}

void SshConsole::switchToAlternateScreen()
{
    isSwitchingToAlternate = true;
    emit onSwitchToAlternateScreen();
}

void SshConsole::keyPressEvent(QKeyEvent *e)
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
        break;
    case Qt::Key_Up:
        emit getData("\033[A");
        break;
    case Qt::Key_Down:
        emit getData("\033[B");
        break;
    case Qt::Key_Home:
        emit getData("\033[H");
        commandParser->setHomePress(true);
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

void SshConsole::inputMethodEvent(QInputMethodEvent *e)
{
    QString commitString = e->commitString();
    if(!commitString.isEmpty())
        emit getData(commitString.toUtf8());
}

void SshConsole::mousePressEvent(QMouseEvent *e)
{
    cursorPos = textCursor().position();
    if(e->buttons() & Qt::LeftButton)
        cancelSelection();

    QPlainTextEdit::mousePressEvent(e);
}

void SshConsole::mouseReleaseEvent(QMouseEvent *e)
{
    QPlainTextEdit::mouseReleaseEvent(e);

    QTextCursor cursor = textCursor();
    setTextColor(palette().color(QPalette::HighlightedText));
    setTextBackgroundColor(palette().color(QPalette::Highlight));
    if(cursor.hasSelection())
    {
        selectStart = cursor.selectionStart();
        selectEnd = cursor.selectionEnd();
        copy();
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
    QPlainTextEdit::mouseDoubleClickEvent(e);
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

QString SshConsole::selectedText()
{
    QString text;
    if(selectStart != selectEnd)
    {
        int pos = selectText();
        setTextColor(palette().color(QPalette::Text));
        setTextBackgroundColor(palette().color(QPalette::Base));
        QTextCursor cursor = textCursor();
        text = cursor.selectedText();
        cursor.setPosition(pos);
        setTextCursor(cursor);
    }
    return text;
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

void SshConsole::removeCurrentRow()
{
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::LineUnderCursor);
    cursor.removeSelectedText();
    setTextCursor(cursor);
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
        int pos = selectText(selectStart, selectEnd);
        setTextColor(palette().color(QPalette::Text));
        setTextBackgroundColor(palette().color(QPalette::Base));
        QTextCursor cursor = textCursor();
        cursor.setPosition(pos);
        setTextCursor(cursor);
        updateSelectedColors();
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

void SshConsole::setTextColor(QColor const& color)
{
    QTextCursor tc = textCursor();
    QTextCharFormat format;
    format.setForeground(color);
    tc.mergeCharFormat(format);
}
void SshConsole::setTextBackgroundColor(QColor const& color)
{
    QTextCursor tc = textCursor();
    QTextCharFormat format;
    format.setBackground(color);
    tc.mergeCharFormat(format);
}

void SshConsole::setTextColor(QColor const& fore, QColor const& back)
{
    QTextCursor tc = textCursor();
    QTextCharFormat format;
    format.setForeground(fore);
    format.setBackground(back);
    tc.mergeCharFormat(format);
}

void SshConsole::setFontFamily(QString const& name)
{
    QTextCursor tc = textCursor();
    QTextCharFormat format;
    format.setFontFamily(name);
    tc.mergeCharFormat(format);
}

void SshConsole::setFontPointSize(int fontSize)
{
    QTextCursor tc = textCursor();
    QTextCharFormat format;
    format.setFontPointSize(fontSize);
    tc.mergeCharFormat(format);
}
