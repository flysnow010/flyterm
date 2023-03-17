#ifndef SSHCONSOLE_H
#define SSHCONSOLE_H
#include <QTextEdit>
#include "core/logfile.h"
#include "color/consolecolor.h"
#include "color/consolepalette.h"

class ConsoleParser;
class QSyntaxHighlighter;
class QMimeData;
class SshConsole : public QTextEdit
{
    Q_OBJECT
public:
    explicit SshConsole(QWidget *parent = nullptr);
    ~SshConsole();

    virtual void putData(const QByteArray &data);
    virtual void clearScreen();

    void setCommandParser(ConsoleParser* parser);
    void connectCommand();
    void disconnectCommand();
    void setLogFile(LogFile::Ptr const& logfile) { logfile_ = logfile; }
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
        ColorRole role;
        int start;
        int end;
    };

signals:
    void getData(const QByteArray &data);
    void onSwitchToAlternateScreen();
    void onSwitchToMainScreen();

public slots:
    void copyAll();
    void clearall();
    void copyOne();
    bool increaseFontSize();
    bool decreaseFontSize();
    void saveToFile();

protected slots:
    void onBeep();
    void onBackspace(int count);
    void onLeft(int count);
    void onRight(int count);
    void onReturn();
    void onHome();
    void onEnd();
    void onDelCharToLineEnd();
    void onText(QString const& text);
    void onForeColor(ColorRole role);
    void onBackColor(ColorRole role);
    void onCleanScreen();
    void onColorClose();

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void insertFromMimeData(const QMimeData *source) override;

    virtual void putText(QString const& text);
    virtual void cursorLeft(int count);
    virtual void setForeColor(ColorRole role);
    virtual void setBackColor(ColorRole role);
    virtual void setCloseColor();
private:
    void clearSelection();
    int selectText();
    int selectText(int start, int end);
    void updateColors();
protected:
    ConsoleParser* commandParser;
    ConsolePalette::Ptr palette_;
    QTextCharFormat textFormat;
    QTextCharFormat normalFormat;
private:
    QSyntaxHighlighter *highlighter = nullptr;
    LogFile::Ptr logfile_;
    ColorRole currentForeRole;
    ColorRole currentBackRole;
    QString fontName_ = "Courier New";
    QList<ColorRange> colorRanges;
    int     fontSize_ = 12;
    QList<int> standardSizes;
    bool isUseColor = false;
    int minCursorPos_ = -1;
    int cursorPos = 0;
    int selectStart = 0;
    int selectEnd = 0;
};

#endif // SSHCONSOLE_H
