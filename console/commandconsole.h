#ifndef COMMANDCONSOLE_H
#define COMMANDCONSOLE_H

#include <QPlainTextEdit>

class CommandConsole : public QPlainTextEdit
{
Q_OBJECT
public:
    CommandConsole(QWidget *parent = nullptr);

signals:
    void getCommand(QString const& command);

public slots:
    void sendCommand();
protected:
    void keyPressEvent(QKeyEvent *e) override;
private:
    bool preCommand();
    bool nextCommand();
private:
    QStringList commands;
    int command_index = 0;
    QString fontName_ = "Courier New";
    int     fontSize_ = 11;
    QTextCharFormat textFormat;
};

#endif // COMMANDCONSOLE_H
