#ifndef CONSOLEPARSER_H
#define CONSOLEPARSER_H
#include "color/consolepalette.h"

#include <QObject>
#include <QByteArray>

class ConsoleParser : public QObject
{
    Q_OBJECT
public:
    ConsoleParser();
    virtual ~ConsoleParser() {}

    enum Code {
        BEL = '\007',
        BS  = '\010',
        SO  = '\016',
        SI  = '\017',
        ESC = '\033',
        SPACE = '\040',
        LF = '\012',//\n
        CR = '\015' //\r
    };

    enum Mode {
        Mode_Insert,
        Mode_Overwrite
    };

    virtual void parse(QByteArray const& data) = 0;
    void setKeyMode(Mode mode) { keyMode_ = mode; }
    Mode keyMode() const { return keyMode_; }
    void setLeftKeyPress(bool enable) { leftKeyPressCount += (enable ? 1 : -1); }
    void setLeftKeyPressCount(int count) {leftKeyPressCount += count; }
    bool isLeftKeyPress() const { return leftKeyPressCount > 0; }
    void setRightKeyPress(bool enable) { isRightKeyPress_ = enable; }
    bool isRightKeyPress() const { return isRightKeyPress_; }
    void setEnterKeyPress(bool enable) { isEnterKeyPress_ = enable; }
    bool isEnterKeyPress() const { return isEnterKeyPress_; }
    void setHomePress(bool enable) { isHomeKeyPress_ = enable; }
    bool isHomeKeyPress() const { return isHomeKeyPress_; }
signals:
    void onBeep();
    void onBackspace(int count);
    void onDelCharToLineEnd();
    void onLeft(int count);
    void onRight(int count);
    void onScreenHome();
    void onHome();
    void onOverWrite(bool enable);
    void onRow(int count);
    void onCol(int count);
    void onCleanScreen();
    void onText(QString const& text);
    void onTitle(QString const& title);
    void onForeColor(ColorRole role);
    void onBackColor(ColorRole role);
    void onForeRgbColor();
    void onBackRgbColor();
    void onBold();
    void onNoBold();
    void onUnderLine();
    void onNoUnderLine();
    void onBlink();
    void onReverse();
    void onNormalColor();
    void onNormalForeColor();
    void onNormalBackColor();
    void onASCIIMode();
    void onDECLineDrawingMode();
    void onColorClose();//onCloseCharAttriutes

    void onSwitchToAlternateCharScreen();
    void onSwitchToAlternateVideoScreen();
    void onSwitchToMainScreen();
    void onSwitchToAppKeypadMode();
    void onSwitchToNormalKeypadMode();
    void onCursorStartBlinking();
    void onCursorStopBlinking();
    void onShowCursor();
    void onHideCursor();
    void onSaveCursorPos();
    void onRestoreCursorPos();
    void onCursorPos(int row, int col);
    void onGetCursorPos();
    void onRowRangle(int top, int bottom);
    void onInsertLine(int lines);//[1P
    void onDeleteLine(int lines);//[1L
    void onScrollDown(int rows);//[1S
    void onScrollUp(int rows);//[1T
    void onSRMOff();
    void onSRMOn();
protected:
    void parseColor(QString const& color);
    void parseSGR(QString const& sgr);
    void parseOneSGR(QString const& sgr);
    void parse_n(QString const& n);
    void parse_C(QString const& c);
    void parse_D(QString const& c);
protected:
    QByteArray parseData_;
private:
    Mode keyMode_ = Mode_Insert;
    int leftKeyPressCount = 0;
    bool isRightKeyPress_ = false;
    bool isEnterKeyPress_ = false;
    bool isHomeKeyPress_ = false;
};

#endif // CONSOLEPARSER_H
