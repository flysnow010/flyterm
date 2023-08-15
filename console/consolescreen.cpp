#include "consolescreen.h"
#include <QTextEdit>
#include <QTextCharFormat>

ConsoleScreen::ConsoleScreen(int cols, int rows)
    : cols_(cols)
    , rows_(rows)
{
    consoleCharsVec.resize(rows);
    for(int i = 0; i < rows_; i++)
        consoleCharsVec[i] = new ConsoleChars(cols_);
}

ConsoleScreen::~ConsoleScreen()
{
    for(int i = 0; i < consoleCharsVec.size(); i++)
        delete consoleCharsVec[i];
}

void ConsoleScreen::clear(bool isAll)
{
    for(int i = 0; i < consoleCharsVec.size(); i++)
        *(consoleCharsVec[i]) = ConsoleChars(consoleCharsVec[i]->size());

    if(isAll)
    {
        top_ = 0;
        bottom_ = 0;
        row_ = 0;
        col_ = 0;
    }
}

void ConsoleScreen::clearScreen()
{
    for(int i = 0; i < consoleCharsVec.size(); i++)
    {
        ConsoleChars* rowData = consoleCharsVec[i];
        for(int j = 0; j < rowData->size(); j++)
            (*rowData)[j].clear(' ', role_);
    }
}

void ConsoleScreen::onCleanToScreenEnd()
{
    for(int i = row_; i < consoleCharsVec.size(); i++)
    {
        ConsoleChars* rowData = consoleCharsVec[i];
        int col = i == row_ ? col_ : 0;
        for(int j = col; j < rowData->size(); j++)
            (*rowData)[j].clear(' ', role_);
    }
}

void ConsoleScreen::setSize(int cols, int rows)
{
    if(cols == cols_ && rows == rows_)
        return;

    for(int i = 0; i < consoleCharsVec.size(); i++)
        delete consoleCharsVec[i];

    consoleCharsVec.resize(rows);
    for(int i = 0; i < rows; i++)
        consoleCharsVec[i] = new ConsoleChars(cols);
    cols_ = cols;
    rows_ = rows;
}

void ConsoleScreen::scrollRangle(int top, int bottom)
{
    if(top >= 1 && bottom >= 1)
    {
        top_ = top - 1;
        bottom_ = bottom - 1;
    }
    else
    {
        top_ = top;
        bottom_ = bottom;
    }
}

void ConsoleScreen::cursorPos(int row, int col)
{
    if(row >= 1 && col >= 1)
    {
        row_ = row - 1;
        col_ = col - 1;
    }
    else
    {
        row_ = row;
        col_ = col;
    }
}

void ConsoleScreen::cursorRow(int row)
{
    if(row >= 1)
        row_ = row - 1;
}

void ConsoleScreen::cursorCol(int col)
{
    if(col >= 1)
        col_ = col - 1;
}

void ConsoleScreen::cursorUp(int count)
{
    if(row_ >= count)
        row_ -= count;
}

void ConsoleScreen::cursorDown(int count)
{
    row_ += count;
}

void ConsoleScreen::cursorRight(int count)
{
    col_ += count;
}

void ConsoleScreen::cursorLeft(int count)
{
    if(col_ >= count)
        col_ -= count;
}

void ConsoleScreen::scrollUp(int rows)
{
    for(int i = 0; i < rows; i++)
        scrollUp();
    for(int i = top_; i < bottom_; i++)
        addUpdateRow(i);
}

void ConsoleScreen::scrollDown(int rows)
{
    for(int i = 0; i < rows; i++)
        scrollDown();
    for(int i = top_; i < bottom_; i++)
        addUpdateRow(i);
}

void ConsoleScreen::scrollUp()
{
    if(bottom_ == 0)
        return;

    for(int i = top_; i < bottom_; i++)
        *(consoleCharsVec[i]) = *(consoleCharsVec[i+1]);
    *(consoleCharsVec[bottom_]) = ConsoleChars(consoleCharsVec[bottom_]->size());
}

void ConsoleScreen::scrollDown()
{
    for(int i = bottom_; i > top_; i--)
        *(consoleCharsVec[i]) = *(consoleCharsVec[i-1]);
    *(consoleCharsVec[top_]) = ConsoleChars(consoleCharsVec[top_]->size());
}

void ConsoleScreen::delCharToLineEnd()
{
   ConsoleChars* rowChars = consoleCharsVec[row_];
   for(int i = col_; i < rowChars->size(); i++)
        (*rowChars)[i].clear(' ', role_);
   addUpdateRow(row_);
}

void ConsoleScreen::delCharToLineHome()
{
    ConsoleChars* rowChars = consoleCharsVec[row_];
    for(int i = 0; i < col_; i++)
        (*rowChars)[i].clear(' ', role_);
    addUpdateRow(row_);
}

void ConsoleScreen::onEraseChars(int count)
{
    ConsoleChars* rowChars = consoleCharsVec[row_];
    for(int i = col_; i < col_ + count && i < rowChars->size(); i++)
    {
        (*rowChars)[i].clear(' ', role_);
    }
    addUpdateRow(row_);
}

void ConsoleScreen::update(QTextEdit* textEdit,
                             ConsolePalette::Ptr const& palette,
                             const QTextCharFormat &text)
{
    textEdit->clear();
    updateRows_.clear();
    QTextCursor tc = textEdit->textCursor();

    for(int i = 0; i < consoleCharsVec.size(); i++)
    {
        ConsoleChars* rowData = consoleCharsVec[i];
        ConsoleText consoleText;
        consoleText.role = (*rowData)[0].role;
        int index = 0;
        int charCount = ((*rowData)[0].isDrawLineMode ? 2 : 1);
        for(int j = 1; j < rowData->size(); j++)
        {
            ConsoleChar const& consoleChar = (*rowData)[j];

            if(consoleChar.role != consoleText.role
                || j == rowData->size() - 1)
            {
                consoleText.text.resize(charCount);
                for(int k = 0; k < consoleText.text.size(); k++)
                {
                    if(!(*rowData)[index].isDrawLineMode)
                        consoleText.text[k] = (*rowData)[index].value;
                    else
                    {
                        QByteArray bytes = drawChar((*rowData)[index].value);
                        if(!bytes.isEmpty())
                        {
                            consoleText.text[k] = bytes[0];
                            consoleText.text[k+1] = bytes[1];
                        }
                        k++;
                    }
                    index++;
                }
                QTextCharFormat colorFormat = text;
                if(consoleText.role.fore != ColorRole::NullRole)
                    colorFormat.setForeground(QBrush(palette->color(consoleText.role.fore).fore));
                if(consoleText.role.back != ColorRole::NullRole)
                    colorFormat.setBackground(QBrush(palette->color(consoleText.role.back).back));
                if(consoleText.role.isReverse)
                {
                    colorFormat.setForeground(Qt::black);
                    colorFormat.setBackground(Qt::white);
                }
                tc.insertText(QString::fromLocal8Bit(consoleText.text), colorFormat);
                consoleText.role = consoleChar.role;
                index = j;
                charCount = (consoleChar.isDrawLineMode ? 2 : 1);
                continue;
            }
            charCount += (consoleChar.isDrawLineMode ? 2 : 1);
        }
        if(i != consoleCharsVec.size() - 1)
            tc.insertText("\n", text);
    }
}

void ConsoleScreen::setText(QString const& text)
{
    int row = row_;
    int col = col_;
    QByteArray localText = text.toLocal8Bit();
    ConsoleChars* rowData = consoleCharsVec[row];
    addUpdateRow(row);
    if(col > 1)
    {
        for(int i = 0; i < col; i++)
        {
            if((*rowData)[i].value == 0)
                (*rowData)[i].value = ' ';
        }
    }
    for(int i = 0; i < localText.size(); i++)
    {
        if(localText[i] == '\r')
            continue;
        if(col < rowData->size() && localText[i] != '\n')
        {
            (*rowData)[col].isDrawLineMode = isDrawLineMode_;
            (*rowData)[col].value = localText[i];
            (*rowData)[col++].role = role_;
        }
        else
        {
            if(row + 1 < consoleCharsVec.size())
            {
                rowData = consoleCharsVec[++row];
                addUpdateRow(row);
            }
            else
            {
                if(row > 1)
                    addUpdateRow(row - 1);
                scrollUp();
            }
            if(localText[i] == '\n')
            {
                if(i == 0 || localText[i - 1] != '\r')
                    continue;
            }
            col = 0;
        }
    }
    row_ = row;
    col_ = col;
}

void ConsoleScreen::drawRow(int row,
                            QTextEdit* textEdit,
                            const ConsolePalette::Ptr &palette,
                            const QTextCharFormat &text)
{
    QTextCursor tc = textEdit->textCursor();
    tc.movePosition(QTextCursor::Start);
    tc.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, row);
    tc.select(QTextCursor::LineUnderCursor);
    ConsoleChars* rowData = consoleCharsVec[row];
    ConsoleText consoleText;
    consoleText.role = (*rowData)[0].role;
    int index = 0;
    int charCount = ((*rowData)[0].isDrawLineMode ? 2 : 1);
    for(int j = 1; j < rowData->size(); j++)
    {
        ConsoleChar const& consoleChar = (*rowData)[j];

        if(consoleChar.role != consoleText.role
            || j == rowData->size() - 1)
        {
            consoleText.text.resize(charCount);
            for(int k = 0; k < consoleText.text.size(); k++)
            {
                if(!(*rowData)[index].isDrawLineMode)
                    consoleText.text[k] = (*rowData)[index].value;
                else
                {
                    QByteArray bytes = drawChar((*rowData)[index].value);
                    if(!bytes.isEmpty())
                    {
                        consoleText.text[k] = bytes[0];
                        consoleText.text[k+1] = bytes[1];
                    }
                    k++;
                }
                index++;
            }
            QTextCharFormat colorFormat = text;
            if(consoleText.role.fore != ColorRole::NullRole)
                colorFormat.setForeground(QBrush(palette->color(consoleText.role.fore).fore));
            if(consoleText.role.back != ColorRole::NullRole)
                colorFormat.setBackground(QBrush(palette->color(consoleText.role.back).back));
            if(consoleText.role.isReverse)
            {
                colorFormat.setForeground(Qt::black);
                colorFormat.setBackground(Qt::white);
            }
            tc.insertText(QString::fromLocal8Bit(consoleText.text), colorFormat);
            consoleText.role = consoleChar.role;
            index = j;
            charCount = (consoleChar.isDrawLineMode ? 2 : 1);
            continue;
        }
        charCount += (consoleChar.isDrawLineMode ? 2 : 1);
    }
    textEdit->setTextCursor(tc);
}

void ConsoleScreen::updateRows(QTextEdit* textEdit, ConsolePalette::Ptr const& palette,
              QTextCharFormat const& text)
{
    foreach(auto row, updateRows_)
        drawRow(row, textEdit, palette, text);
    updateRows_.clear();
}

void ConsoleScreen::addUpdateRow(int row)
{
    if(!updateRows_.contains(row))
        updateRows_ << row;
}

QByteArray ConsoleScreen::drawChar(char ch)
{
    if(ch == 'j')
        return QString("┘").toLocal8Bit();
    if(ch == 'k')
        return QString("┐").toLocal8Bit();
    if(ch == 'l')
        return QString("┌").toLocal8Bit();
    if(ch == 'm')
        return QString("└").toLocal8Bit();
    if(ch == 'n')
        return QString("┼").toLocal8Bit();
    if(ch == 'q')
        return QString("─").toLocal8Bit();
    if(ch == 't')
        return QString("├").toLocal8Bit();
    if(ch == 'u')
        return QString("┤").toLocal8Bit();
    if(ch == 'v')
        return QString("┴").toLocal8Bit();
    if(ch == 'w')
        return QString("┬").toLocal8Bit();
    if(ch == 'x')
        return QString("│").toLocal8Bit();
    return QByteArray();
}
