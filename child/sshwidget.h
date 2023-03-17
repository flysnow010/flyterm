#ifndef SSHWIDGET_H
#define SSHWIDGET_H

#include <QWidget>
#include <sshremoteprocessrunner.h>
#include "core/logfile.h"
#include "color/consolecolor.h"
#include "color/consolepalette.h"

class QMenu;
class ConsoleParser;
class SshConsole;
class AlternateConsole;
class SSHSettings;
class CommandThread;
class SShWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SShWidget(bool isLog, QWidget *parent = nullptr);
    ~SShWidget();

    void setAskpassFilePath(QString const& filePath);
    void setSshFilePath(QString const& filePath);
    QString sshFilePath();
    bool runShell(SSHSettings const& settings);
    void sendCommand(QString const& command);

    QSize sizeHint() const override;

    void disconnect();
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
    void connectionError();
    void processStarted();
    void readyReadStandardOutput();
    void readyReadStandardError();
    void processClosed(const QString &error);
    void writeData(QByteArray const&data);

    void customContextMenu(const QPoint &);
    void setHighLighter();

    void switchToAlternateScreen();
    void switchToMainScreen();
    void execCommand(QString const& command);
private:
    void createHighLightMenu(QMenu* menu);
    void sendCommands(QStringList const& commands);
private:
    SshConsole* console;
    AlternateConsole* alternateConsole;
    CommandThread* commandThread_;
    ConsoleParser* commandParser;
    QSsh::SshRemoteProcessRunner * shell;
    LogFile::Ptr logfile_;
    bool sheelIsClose = true;
    bool isMainScreen = true;
    QString highLight_;
};

#endif // SSHWIDGET_H
