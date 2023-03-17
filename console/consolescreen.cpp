#include "consolescreen.h"
#include <QTextEdit>
#include <QTextCharFormat>

ConsoleScreen::ConsoleScreen(int rows, int cols)
    : rows_(rows)
    , cols_(cols)
{
    consoleCharsVec.resize(rows);
    for(int i = 0; i < rows; i++)
        consoleCharsVec[i] = new ConsoleChars(cols);
}

ConsoleScreen::~ConsoleScreen()
{
    for(int i = 0; i < consoleCharsVec.size(); i++)
        delete consoleCharsVec[i];
}

void ConsoleScreen::clear()
{
    for(int i = 0; i < consoleCharsVec.size(); i++)
        *(consoleCharsVec[i]) = ConsoleChars(consoleCharsVec[i]->size());

    top_ = 0;
    bottom_ = 0;
    row_ = 0;
    col_ = 0;
}

void ConsoleScreen::scrollRangle(int top, int bottom)
{
    top_ = top - 1;
    bottom_ = bottom - 1;
}

void ConsoleScreen::cursorPos(int row, int col)
{
    row_ = row - 1;
    col_ = col - 1;
}

void ConsoleScreen::cursorRow(int row)
{
    row_ = row - 1;
}

void ConsoleScreen::cursorCol(int col)
{
    col_ = col - 1;
}

void ConsoleScreen::cursorUp(int count)
{
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
    col_ -= count;
}

void ConsoleScreen::scrollUp(int lines)
{
    for(int i = 0; i < lines; i++)
        scrollUp();
}

void ConsoleScreen::scrollDown(int lines)
{
    for(int i = 0; i < lines; i++)
        scrollDown();
}

void ConsoleScreen::scrollUp()
{
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
        (*rowChars)[i].value = QChar::Null;
}

void ConsoleScreen::drawText(QTextEdit* textEdit, ConsolePalette::Ptr const& palette, const QTextCharFormat &text)
{
    textEdit->clear();
    QTextCursor tc = textEdit->textCursor();

    for(int i = 0; i < consoleCharsVec.size(); i++)
    {
        ConsoleChars* rowData = consoleCharsVec[i];
        ConsoleText consoleText;
        consoleText.role = (*rowData)[0].role;
        int index = 0;
        for(int j = 1; j < rowData->size(); j++)
        {
            ConsoleChar const& consoleChar = (*rowData)[j];

            if(consoleChar.role != consoleText.role
                || j == rowData->size() - 1)
            {
                consoleText.text.resize(j-index);
                for(int k = 0; k < consoleText.text.size(); k++)
                    consoleText.text[k] = (*rowData)[index + k].value;
                QTextCharFormat colorFormat = text;
                if(consoleText.role.fore != ColorRole::NullRole)
                    colorFormat.setForeground(QBrush(palette->color(consoleText.role.fore).fore));
                if(consoleText.role.back != ColorRole::NullRole)
                    colorFormat.setBackground(QBrush(palette->color(consoleText.role.back).back));
                tc.insertText(consoleText.text, colorFormat);
                consoleText.role = consoleChar.role;
                index = j;
            }
        }
        if(i != consoleCharsVec.size() - 1)
            tc.insertText("\n", text);
    }
}

void ConsoleScreen::setText(QString const& text)
{
    int row = row_;
    int col = col_;
    ConsoleChars* rowData = consoleCharsVec[row];
    if(col > 1)
    {
        for(int i = 0; i < col; i++)
        {
            if((*rowData)[i].value == QChar::Null)
            (*rowData)[i].value = QChar(' ');
        }
    }
    for(int i = 0; i < text.size(); i++)
    {
        if(text[i] == QChar('\r'))
            continue;
        if(col < rowData->size() && text[i] != QChar('\n'))
        {
            if(isDrawLineMode_)
                (*rowData)[col].value = drawChar(text[i]);
            else
                (*rowData)[col].value = text[i];
            (*rowData)[col++].role = role_;
        }
        else
        {
            rowData = consoleCharsVec[++row];
            col = 0;
        }
    }
    row_ = row;
    col_ = col;
}

QChar ConsoleScreen::drawChar(QChar ch)
{
    if(ch == QChar('j'))
        return QString("┘").at(0);
    if(ch == QChar('k'))
        return QString("┐").at(0);
    if(ch == QChar('l'))
        return QString("┌").at(0);
    if(ch == QChar('m'))
        return QString("└").at(0);
    if(ch == QChar('n'))
        return QString("┼").at(0);
    if(ch == QChar('q'))
        return QString("─").at(0);
    if(ch == QChar('t'))
        return QString("├").at(0);
    if(ch == QChar('u'))
        return QString("┤").at(0);
    if(ch == QChar('v'))
        return QString("┴").at(0);
    if(ch == QChar('w'))
        return QString("┬").at(0);
    if(ch == QChar('x'))
        return QString("│").at(0);
    return ch;
}
