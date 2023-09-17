#ifndef LOCALSHELLWIDGET_H
#define LOCALSHELLWIDGET_H

#include <QWidget>

#include "core/logfile.h"
#include "color/consolecolor.h"
#include "color/consolepalette.h"

class QMenu;
class Console;
class LocalShell;
class LocalShellSettings;
class CommandThread;
class LocalShellWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LocalShellWidget(bool isLog, QWidget *parent = nullptr);

    bool runShell(LocalShellSettings const& settings);
    void sendCommand(QString const& command);

    QSize sizeHint() const override;

    bool isConnected() const;
    void reconnect(LocalShellSettings const& settings);

    void disconnect();
    bool isDisplay() const;
    void display();
    void undisplay();
    void setCodecName(QString const& name);
    void setFontName(QString const& name);
    void setConsoleColor(ConsoleColor const& color);
    void setConsolePalette(ConsolePalette::Ptr palette);
    void setFontSize(int fontSize);
    void print();
    void updateHightLighter(QString const& hightLighter);
    void setHighLighter(QString const& hightLighter);
public slots:
    void save();
    void copy();
    void copyAll();
    void paste();
    void increaseFontSize();
    void decreaseFontSize();
    void clearScrollback();
signals:
    void onClose(QWidget *widget);
    void onCommand(QString const& command);
    void fontSizeChanged(int fonstSize);
    void getHighlighter();
    void highLighterChanged(QString const& highLight);
protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
private slots:
    void onData(QByteArray const& data);
    void writeData(QByteArray const&data);
    void customContextMenu(const QPoint &);
    void setHighLighter();
    void onGotCursorPos(int row, int col);
    void execCommand(QString const& command);
    void execTestCommand(QString const& command);
    void execExpandCommand(QString const& command);
    void pullData();
    void setCommand(QString const& command);
private:
    void createHighLightMenu(QMenu* menu);
    void sendCommands(QStringList const& commands);
    QString getTestCommand();
    bool testCommandIsEmpty() const;
private:
    Console* console;
    CommandThread* commandThread;
    LocalShell* shell;
    QTimer* dataTimer;
    QStringList texts;
    LogFile::SharedPtr beforeLogfile_;
    LogFile::SharedPtr afterLogfile_;
    QString highLight_;
    QStringList testCommands_;
    QByteArray testParam_;
    bool isTest_ = false;
    bool isConnected_ = false;
    QByteArray testData_;
    QString command_;
};

#endif // LOCALSHELLWIDGET_H
