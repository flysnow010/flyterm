#ifndef SSHWIDGET_H
#define SSHWIDGET_H

#include <QWidget>
#include "core/sshshell.h"
#include "core/sshsettings.h"
#include "core/logfile.h"
#include "color/consolecolor.h"
#include "color/consolepalette.h"

class QMenu;
class ConsoleParser;
class SshConsole;
class AlternateConsole;
class CommandThread;
class QTimer;
class SShWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SShWidget(bool isLog, QWidget *parent = nullptr);
    ~SShWidget();

    bool runShell(SSHSettings const& settings);
    void sendCommand(QString const& command);
    void activedWidget();

    QSize sizeHint() const override;

    bool isConnected() const;
    void reconnect(SSHSettings const& settings);
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
    void uploadFile();
    void downloadFile();
signals:
    void onClose(QWidget *widget);
    void onConnectStatus(QWidget *widget, bool on);
    void onSizeChanged(QWidget *widget);
    void onCommand(QString const& command);
    void onTitle(QString const& title);
    void fontSizeChanged(int fonstSize);
    void getHighlighter();
    void highLighterChanged(QString const& highLight);
protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
private slots:
    void connected();
    void connectionError(QString const& error);
    void onData(QByteArray const& data);
    void onError(QByteArray const& data);
    void writeData(QByteArray const&data);
    void onGotCursorPos(int row, int col);

    void customContextMenu(const QPoint &);
    void setHighLighter();

    void switchToAlternateScreen();
    void switchToAppKeypadMode();
    void switchToNormalKeypadMode();
    void switchToMainScreen();
    void execCommand(QString const& command);
    void execTestCommand(QString const& command);
    void execExpandCommand(QString const& command);
    void pullData();
    void secondaryDA();
private:
    void createHighLightMenu(QMenu* menu);
    void sendCommands(QStringList const& commands);
    void getShellSize(QSize const& size, int &cols, int &rows);
    QString getTestCommand();
    bool testCommandIsEmpty() const;
    QString  getCurrentPath();
    void transferFile(QString const& srcFileName, QString const& dstFileName, bool isUpload);
private:
    SshConsole* console;
    AlternateConsole* alternateConsole;
    CommandThread* commandThread_;
    ConsoleParser* commandParser;
    SshShell * shell;
    QTimer* dataTimer;
    QSize consoleSize;
    int shellRows = 0;
    int shellCols = 0;
    LogFile::SharedPtr beforeLogfile_;
    LogFile::SharedPtr afterLogfile_;
    bool sheelIsClose = true;
    bool isMainScreen = true;
    QString highLight_;
    QStringList testCommands_;
    QByteArray testParam_;
    bool isTest_ = false;
    QByteArray testData_;
    SSHSettings settings_;
    QString filePath;
};

#endif // SSHWIDGET_H
