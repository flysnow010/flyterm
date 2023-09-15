#ifndef SSHCONSOLE_H
#define SSHCONSOLE_H
#include <QPlainTextEdit>
#include "core/logfile.h"
#include "color/consolecolor.h"
#include "color/consolepalette.h"
#include "console/consolescreen.h"

class ConsoleParser;
class QSyntaxHighlighter;
class QMimeData;
class SshConsole : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit SshConsole(QWidget *parent = nullptr);
    ~SshConsole();

    virtual void putData(const QByteArray &data);

    void setCommandParser(ConsoleParser* parser);
    void connectCommand();
    void disconnectCommand();
    void setLogFile(LogFile::SharedPtr const& logfile) { logfile_ = logfile; }
    void setFontName(QString const& fontName);
    void setFontSize(int fontSize);
    void setConsoleColor(ConsoleColor const& color);
    void setConsolePalette(ConsolePalette::Ptr palette);

    QString fontName() const { return fontName_; }
    int fontSize() const { return fontSize_; }
    void updateHightLighter(QString const& hightLighter);

    void cancelSelection();

    struct ColorRange
    {
        ColorRole fore;
        ColorRole back;
        int start;
        int end;
    };
    QString selectedText();
signals:
    void getData(const QByteArray &data);
    void onGotCursorPos(int row, int col);
    void onTitle(QString const& title);
    void onSwitchToAlternateScreen();
    void onSwitchToAlternateFinished();
    void onSwitchToAppKeypadMode();
    void onSwitchToNormalKeypadMode();
    void onSaveCursorPos();
    void onRestoreCursorPos();
    void onSwitchToMainScreen();
    void onSecondaryDA();

public slots:
    void copyAll();
    void clearall();
    void copyOne();
    bool increaseFontSize();
    bool decreaseFontSize();
    void saveToFile();

protected slots:
    void onBeep();
    void onGetCursorPos();
    void onBackspace(int count);
    void onDelChars(int count);
    void onLeft(int count);
    void onRight(int count);
    void onReturn();
    void onHome();
    void onEnd();
    void onOverWrite(bool enable);
    void onDelCharToLineEnd();
    void onText(QString const& text);
    void onForeColor(ColorRole role);
    void onBackColor(ColorRole role);
    void onReverse();
    void onBold(bool enable);
    void onUnderLine(bool enable);
    void onCleanScreen();
    void onCloseCharAttriutes();
    void onRestoreState();
    void switchToAlternateScreen();

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void inputMethodEvent(QInputMethodEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void insertFromMimeData(const QMimeData *source) override;

    virtual void putText(QString const& text);
    virtual void home();
    virtual void backspace(int count);
    virtual void delChars(int count);
    virtual void cursorLeft(int count);
    virtual void cursorRight(int count);
    virtual void setForeColor(ColorRole role);
    virtual void setBackColor(ColorRole role);
    virtual void setReverse();
    virtual void setBold(bool enable);
    virtual void setUnderLine(bool enable);
    virtual void setCloseColor();
    virtual void clearScreen();
    virtual void delCharToLineEnd();
private:
    void removeCurrentRow();
    void clearSelection();
    int selectText();
    int selectText(int start, int end);
    void updateColors();
    void updateSelectedColors();
    void setTextColor(QColor const& color);
    void setTextBackgroundColor(QColor const& color);
    void setTextColor(QColor const& fore, QColor const& back);
    void setFontFamily(QString const& name);
    void setFontPointSize(int fontSize);
protected:
    ConsoleParser* commandParser;
    ConsolePalette::Ptr palette_;
    QTextCharFormat textFormat;
    QTextCharFormat normalFormat;
    int cursorPos = 0;
private:
    QSyntaxHighlighter *highlighter = nullptr;
    LogFile::WeakPtr logfile_;
    ColorRole currentForeRole;
    ColorRole currentBackRole;
    QString fontName_ = "Courier New";
    QList<ColorRange> colorRanges;
    int     fontSize_ = 12;
    QList<int> standardSizes;
    bool isUseColor = false;
    bool isOverWrite = false;
    bool isSwitchingToAlternate = false;
    int minCursorPos_ = -1;
    bool isReturn = false;
    int selectStart = 0;
    int selectEnd = 0;
    ConsoleScreen screen;
};

#endif // SSHCONSOLE_H
