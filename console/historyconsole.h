#ifndef HISTORYCONSOLE_H
#define HISTORYCONSOLE_H

#include <QTextBrowser>

class HistoryConsole : public QTextBrowser
{
Q_OBJECT
public:
    HistoryConsole(QWidget *parent = nullptr);

    void saveToFile(QString const& fileName);
    void loadFromFile(QString const& fileName);
private:
    QStringList commands;
    int command_index = 0;
    QString fontName_ = "Courier New";
    int     fontSize_ = 10;
    QTextCharFormat textFormat;
};

#endif // HISTORYCONSOLE_H
