#ifndef CONSOLESCREEN_H
#define CONSOLESCREEN_H
#include <QVector>
#include <QChar>

#include "consolechar.h"
class QTextEdit;
class QTextCharFormat;

class ConsoleScreen
{
public:
    ConsoleScreen(int cols = 100, int rows = 24);
    ConsoleScreen(ConsoleScreen const&) = delete;
    void operator =(ConsoleScreen const&) = delete;

    ~ConsoleScreen();

    void clear(bool isAll);
    void setSize(int cols, int rows);
    int rows() const { return rows_; }
    int cols() const { return cols_; }
    int row() const { return row_ + 1; }
    int col() const { return col_ + 1; }

    void cursorPos(int row, int col);
    void cursorRow(int row);
    void cursorCol(int col);
    void cursorUp(int count);
    void cursorDown(int count);
    void cursorRight(int count);
    void cursorLeft(int count);

    void scrollRangle(int top, int bottom);
    void scrollUp(int rows);
    void scrollDown(int rows);

    void delCharToLineEnd();

    void setForeColor(ColorRole fore) { role_.fore = fore; }
    void setBackColor(ColorRole back) { role_.back = back; }

    void setText(QString const& text);
    void updateRows(QTextEdit* textEdit, ConsolePalette::Ptr const& palette,
                  QTextCharFormat const& text);
    void update(QTextEdit* textEdit, ConsolePalette::Ptr const& palette,
                  QTextCharFormat const& text);

    void setDrawLineMode(bool enable) { isDrawLineMode_ = enable; }

    bool isBottom() const { return bottom_ == row_; }
private:
    typedef QVector<ConsoleChar> ConsoleChars;
    void scrollUp();
    void scrollDown();
    QByteArray drawChar(char ch);
    void drawRow(int row, QTextEdit* textEdit, ConsolePalette::Ptr const& palette,
                 QTextCharFormat const& text);
    void addUpdateRow(int row);
private:
    QVector<ConsoleChars *> consoleCharsVec;
    ConsolRole role_;
    QList<int> updateRows_;
    bool isDrawLineMode_ = false;
    int top_ = 0;
    int bottom_ = 0;
    int col_ = 0;
    int row_ = 0;
    int cols_;
    int rows_;
};

#endif // CONSOLESCREEN_H
