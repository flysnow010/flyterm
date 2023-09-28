#ifndef CHILD_H
#define CHILD_H

#include <QWidget>
#include "core/logfile.h"
#include "color/consolecolor.h"
#include "color/consolepalette.h"

class QMenu;
class Console;
class CommandThread;
class Child : public QWidget
{
    Q_OBJECT
public:
    explicit Child(QWidget *parent = nullptr);

    void sendCommand(QString const& command);
    QSize sizeHint() const override;

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
    void onTitle(QString const& title);
    void fontSizeChanged(int fonstSize);
    void getHighlighter();
    void highLighterChanged(QString const& highLight);
protected:
    virtual Console* createConsole() = 0;
    virtual void writeToShell(QByteArray const& data) = 0;
    virtual void onPullData() = 0;
    virtual void onContextMenu(QMenu &) {}
    virtual void onExecCommand(QString const& command);
    virtual void onExpandCommand(QString const&) {}

    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void createConsoleAndConnect();
    void onDisplay(QByteArray const& data);
    void setConsoleText(QString const& text);
    Console* console() { return console_; }
private slots:
    void customContextMenu(const QPoint &);
    void setHighLighter();
    void onGotCursorPos(int row, int col);
    void execCommand(QString const& command);
    void execTestCommand(QString const& command);
    void execExpandCommand(QString const& command);
    void pullData();
private:
    void createHighLightMenu(QMenu* menu);
    void sendCommands(QStringList const& commands);
    QString getTestCommand();
    bool testCommandIsEmpty() const { return testCommands_.isEmpty(); }
private:
    Console* console_ = nullptr;
    CommandThread* commandThread;
    QTimer* dataTimer;
    LogFile::SharedPtr afterLogfile_;
    QString highLight_;
    QStringList testCommands_;
    QByteArray testParam_;
    bool isTest_ = false;
    QByteArray testData_;
};

#endif // CHILD_H
