#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTranslator>

#include "core/basesession.h"
#include "core/connecttype.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QMdiArea;
class QMdiSubWindow;
class QFontComboBox;
class QComboBox;
class QToolButton;
class QActionGroup;
class SessionDockWidget;
class ButtonsDockWidget;
class CommandDockWidget;
class TFtpServer;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    enum WindowMode { Max, Tile, Cascade };

    static void LoadSettings();
    static void InstallTranstoirs(bool isInited = false);
protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newConnect();
    void midViewMode();
    void updateStatus(QMdiSubWindow *window);
    void updateFontSize(int fontSize);
    void createShell(Session::Ptr & session);

    void tileChildWindow();
    void cascadeChildWindow();
    void maximizeChildWidnow();
    void cancelconnect();

    void save();
    void print();
    void copy();
    void copyAll();
    void udpateHighLighter(QString const& lighter);
    void selectCodec();
    void setFontName(QString const& name);
    void setFontSize(QString const& fontSize);
    void setColorIndex(int index);
    void setPaletteName(QString const& name);
    void increaseFontSize();
    void decreaseFontSize();
    void clearScrollback();
    void paste();
    void sendCommand(QString const& command);

    void logToggle(bool isChecked);
    void setHighLighter();
    void subWindowStateChanged(Qt::WindowStates oldState,
                               Qt::WindowStates newState);
    void windowTitleChanged(const QString &title);

    void showStatusText(QString const& text);
    void tftpServerStart();
    void tftpServerStop();
private:
    void sendFile(QString const& protocol);
    void recvFile(QString const& protocol);
    void createConnets();
    void createDockWidgets();
    void createMenus();
    void createHighLighterMenu();
    void udpateHighLighterMenuStatus(QString const& lighter);
    void createToolButtons();
    void addSession(bool isCreateSheel);
    void updateWindowState();
    void readSettings();
    void writeSettings();
    static void SaveSettings();
    QMdiSubWindow* activeSubWindow();
    Session::Ptr activeSession();
    void loadStyleSheet();
    void setShowStyle(QString const& style);
    void setLanguage(QString const& lang);
    void retranslateUi();
private:
    Ui::MainWindow *ui;
    QMdiArea *mdiArea;
    static QTranslator appTranslator;
    static QTranslator sysTranslator;
    static QString     language;
    static QString     showStyle;
    SessionDockWidget* sessionDockWidget;
    ButtonsDockWidget* buttonsDockWidget;
    CommandDockWidget* commandDockWidget;
    TFtpServer* tftpServer_;
    QActionGroup* windowGroup;
    QActionGroup* hightlightGroup;
    QActionGroup* languageGroup;
    QActionGroup* showstyleGroup;
    QAction* statusBarAction;
    QComboBox* comboColor;
    QComboBox* comboPalette;
    QFontComboBox* comboFont;
    QComboBox* comboSize;
    QToolButton* buttonCodec;
    WindowMode windowMode = Max;
    ConnectType connectType = ConnectType::SSH;
    QString tftpRootPath;
    bool isLog = false;
};
#endif // MAINWINDOW_H
