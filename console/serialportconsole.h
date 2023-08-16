#ifndef SERIALPORTCONSOLE_H
#define SERIALPORTCONSOLE_H

#include <QTextEdit>
#include "core/logfile.h"
#include "color/consolecolor.h"
#include "console/consolescreen.h"

class ConsoleParser;
class QSyntaxHighlighter;
class QMimeData;
class SerialPortConsole : public QTextEdit
{
    Q_OBJECT
public:
    explicit SerialPortConsole(QWidget *parent = nullptr);
    ~SerialPortConsole();

    void putData(const QByteArray &data);
    void setLocalEchoEnabled(bool set);
    void setLogFile(LogFile::SharedPtr const& logfile) { logfile_ = logfile; }
    void setCodecName(QString const& name);
    void setFontName(QString const& name);
    void setFontSize(int fontSize);
    void setConsoleColor(ConsoleColor const& color);
    void setConsolePalette(ConsolePalette::Ptr palette);

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
    void onGotCursorPos(int row, int col);
public slots:
    void copyAll();
    void clearall();
    void copyOne();
    bool increaseFontSize();
    bool decreaseFontSize();
    void saveToFile();
protected:
    void keyPressEvent(QKeyEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void insertFromMimeData(const QMimeData *source) override;
private slots:
    void onBeep();
    void onGetCursorPos();
    void onBackspace(int count);
    void onLeft(int count);
    void onRight(int count);
    void onEnd();
    void onText(QString const& text);
    void onForeColor(ColorRole role);
    void onColorClose();
private:
    void removeCurrentRow();
    void clearSelection();
    int selectText();
    int selectText(int start, int end);
    void updateColors();
    void setForeColor(ColorRole role);
    void setCloseColor();
    void putText(QString const& text);
private:
    ConsoleParser *commandParser;
    ConsolePalette::Ptr palette_;
    QSyntaxHighlighter *highlighter = nullptr;
    LogFile::WeakPtr logfile_;
    ColorRole currentForeRole;
    ColorRole currentBackRole;
    QString fontName_ = "Courier New";
    int     fontSize_ = 12;
    QList<ColorRange> colorRanges;
    QList<int> standardSizes;
    bool isUseColor = false;
    QTextCharFormat textFormat;
    QTextCharFormat normalFormat;
    bool m_localEchoEnabled = false;
    bool isReturn = false;
    int cursorPos = 0;
    int selectStart = 0;
    int selectEnd = 0;
    ConsoleScreen screen;
};

#endif // SERIALPORTCONSOLE_H
