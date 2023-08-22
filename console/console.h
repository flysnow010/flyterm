#ifndef CONSOLE_H
#define CONSOLE_H

#include <QPlainTextEdit>
#include "core/logfile.h"
#include "color/consolerole.h"
#include "console/consolescreen.h"

class ConsoleParser;
class QSyntaxHighlighter;
class QMimeData;
class Console : public QPlainTextEdit
{
    Q_OBJECT
public:
    Console(QWidget *parent = nullptr);
    virtual ~Console();

    void putData(const QByteArray &data);

    void setLogFile(LogFile::SharedPtr const& logfile) { logfile_ = logfile; }
    void setLocalEchoEnabled(bool enabled) { localEchoEnabled = enabled; }
    void setCodecName(QString const& name);
    void setFontName(QString const& name);
    void setFontSize(int fontSize);
    void setConsoleColor(ConsoleColor const& color);
    void setConsolePalette(ConsolePalette::Ptr palette);

    void updateHightLighter(QString const& hightLighter);
    void cancelSelection();

    struct ColorRange
    {
        ColorRole role;
        int start;
        int end;
    };

    virtual void connectCommands();
    virtual void disconnectCommands();
public slots:
    void copyAll();
    void clearall();
    void copyOne();
    bool increaseFontSize();
    bool decreaseFontSize();
    void saveToFile();

signals:
    void getData(const QByteArray &data);
    void onGotCursorPos(int row, int col);
protected:
    void keyPressEvent(QKeyEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void insertFromMimeData(const QMimeData *source) override;
    virtual ConsoleParser* createParser() = 0;
private slots:
    void onBeep();
    void onGetCursorPos();
    void onBackspace(int count);
    void onLeft(int count);
    void onRight(int count);
    void onText(QString const& text);
    void onForeColor(ColorRole role);
    void onBackColor(ColorRole role);//???
    void onColorClose();
private:
    void onEnd();

    void removeCurrentRow();
    void clearSelection();
    void putText(QString const& text);

    int selectText();
    int selectText(int start, int end);
    void updateColors();
    void setForeColor(ColorRole role);
    void setBackColor(ColorRole role);
    void setCloseColor();
private:
    ConsoleParser *commandParser = 0;
    ConsolePalette::Ptr palette_;
    QSyntaxHighlighter *highlighter = nullptr;
    LogFile::WeakPtr logfile_;
    ColorRole currentForeRole;
    ColorRole currentBackRole;

    QList<int> standardSizes;
    bool isUseColor = false;
    QTextCharFormat textFormat;
    QTextCharFormat normalFormat;
    QString fontName_ = "Courier New";
    int     fontSize_ = 12;

    bool isReturn = false;
    bool localEchoEnabled = false;
    QList<ColorRange> colorRanges;
    int cursorPos = 0;
    int selectStart = 0;
    int selectEnd = 0;

    ConsoleScreen screen;
};

#endif // CONSOLE_H
