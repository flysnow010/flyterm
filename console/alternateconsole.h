#ifndef ALTERNATECONSOLE_H
#define ALTERNATECONSOLE_H

#include "console/sshconsole.h"
#include "console/consolescreen.h"

class AlternateConsole : public SshConsole
{
    Q_OBJECT
public:
    AlternateConsole(QWidget *parent = nullptr);

    void connectAppCommand();
    void disconnectAppCommand();
    void putData(const QByteArray &data) override;
    void reset();
    void shellSize(int cols, int rows);

public slots:
    void updateRows();
protected:
    void keyPressEvent(QKeyEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void putText(QString const& text) override;
    void setForeColor(ColorRole role) override;
    void setBackColor(ColorRole role) override;
    void setReverse() override;
    void setBold(bool enable) override;
    void setUnderLine(bool enable) override;
    void setCloseColor() override;
    void home() override;
    void backspace(int count) override;
    void delChars(int count) override;
    void cursorLeft(int count) override;
    void cursorRight(int count) override;
    void clearScreen() override;
    void delCharToLineEnd() override;
private slots:
    void onSwitchToAppKeypadMode();
    void onSwitchToNormalKeypadMode();
    void onASCIIMode();
    void onDECLineDrawingMode();
    void onRowRangle(int top, int bottom);
    void onScreenHome();
    void onCursorPos(int row, int col);
    void onRow(int row);
    void onCol(int col);
    void onUp(int line);
    void showCursor();
    void hideCursor();
    void scrollDown(int rows);
    void scrollUp(int rows);
    void insertLine(int lines);
    void deleteLine(int lines);
    void delCharToLineHome();
    void insertChars(int count);
    void onEraseChars(int count);
    void onCleanToScreenEnd();
private:
    int topRow = -1;
    int bottomRow = -1;
    int cursorWidth = 0;
    bool isDebug = false;
    ConsoleScreen screen;
};

#endif // ALTERNATECONSOLE_H
