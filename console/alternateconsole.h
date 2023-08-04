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
    void reset(bool video);
    void shellSize(int cols, int rows);
protected:
    void keyPressEvent(QKeyEvent *e) override;
    void putText(QString const& text) override;
    void setForeColor(ColorRole role) override;
    void setBackColor(ColorRole role) override;
    void setCloseColor() override;
    void home() override;
    void backspace(int count) override;
    void cursorLeft(int count) override;
    void cursorRight(int count) override;
    void clearScreen() override;
private slots:
    void onSwitchToAppKeypadMode();
    void onSwitchToNormalKeypadMode();
    void onASCIIMode();
    void onDECLineDrawingMode();
    void onRowRangle(int top, int bottom);
    void onScreenHome();
    void onCursorPos(int row, int col);
    void showCursor();
    void hideCursor();
    void scrollDown(int rows);
    void scrollUp(int rows);
    void delCharToLineEnd();
private:
    void down();
private:
    int topRow = -1;
    int bottomRow = -1;
    bool isPutText = true;
    bool isUpdate = false;
    bool isVideo = false;
    ConsoleScreen screen;
};

#endif // ALTERNATECONSOLE_H
