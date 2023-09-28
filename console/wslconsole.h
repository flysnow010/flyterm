#ifndef WSLCONSOLE_H
#define WSLCONSOLE_H

#include "console.h"

class WSLConsole : public Console
{
    Q_OBJECT
public:
    WSLConsole(QWidget *parent = nullptr);

    void connectCommands() override;
    void disconnectCommands() override;

protected:
     ConsoleParser* createParser() override;
private slots:
     void hideCursor();
     void cleanScreen();
     void screenHome();
     void cursorPos(int row, int col);
     void eraseChars(int n);
     void right(int n);
     void showCursor();
private:
     int cursorWidth_ = 0;
};

#endif // WSLCONSOLE_H
