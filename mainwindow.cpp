#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "child/sshwidget.h"
#include "child/serialportwidget.h"
#include "dock/sessiondockwidget.h"
#include "dock/buttonsdockwidget.h"
#include "dock/commanddockwidget.h"
#include "dialog/connectdialog.h"
#include "dialog/aboutdialog.h"
#include "core/sshsession.h"
#include "core/telnetsession.h"
#include "core/serialsession.h"
#include "core/localshellsession.h"
#include "core/wslsession.h"
#include "core/userauth.h"
#include "core/languagemanager.h"
#include "core/consolecodec.h"
#include "service/tftp/tftpserver.h"
#include "color/consolepalette.h"
#include "highlighter/hightlightermanager.h"
#include "util/util.h"

#include <QMdiArea>
#include <QMdiSubWindow>
#include <QFontComboBox>
#include <QToolButton>
#include <QDesktopWidget>
#include <QSettings>
#include <QLocalSocket>
#include <QActionGroup>
#include <QFileDialog>
#include <QStyleFactory>
#include <QTimer>

QTranslator MainWindow::appTranslator;
QTranslator MainWindow::sysTranslator;
QString MainWindow::language;
QString MainWindow::showStyle;

int const SHOW_WINDOW_TIME_OUT = 100;

void MainWindow::InstallTranstoirs(bool isInited)
{
    Language lang;
    LanguageManager().find(language, lang);

    if(isInited)
    {
        qApp->removeTranslator(&appTranslator);
        qApp->removeTranslator(&sysTranslator);
    }

    if(!lang.isDefault())
    {
        appTranslator.load(QString("%1/%2.qm").arg(Util::languagePath(), lang.file));
        sysTranslator.load(QString("%1/qt_%2.qm").arg(Util::languagePath(), lang.file));
        qApp->installTranslator(&appTranslator);
        qApp->installTranslator(&sysTranslator);
    }
}

void MainWindow::LoadSettings()
{
    QSettings settings(QCoreApplication::applicationName(),
                       QCoreApplication::applicationVersion());
    language = settings.value("language", "English").toString();
    showStyle = settings.value("showStyle", "WindowsVista").toString();
    QApplication::setStyle(QStyleFactory::create(showStyle));
}

void MainWindow::SaveSettings()
{
    QSettings settings(QCoreApplication::applicationName(),
                       QCoreApplication::applicationVersion());

    settings.setValue("language", language);
    settings.setValue("showStyle", showStyle);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mdiArea(new QMdiArea)
    , sessionDockWidget(new SessionDockWidget(this))
    , buttonsDockWidget(new ButtonsDockWidget(this))
    , commandDockWidget(new CommandDockWidget(this))
    , tftpServer_(new TFtpServer(this))
    , windowGroup(new QActionGroup(this))
    , hightlightGroup(new QActionGroup(this))
    , languageGroup(new QActionGroup(this))
    , showstyleGroup(new QActionGroup(this))
    , statusBarAction(0)
{
    ui->setupUi(this);
    highLighterManager->init();
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setDocumentMode(true);
    mdiArea->setTabsClosable(true);
    setDiaplay(true);
    setConnect(true);
    setCentralWidget(mdiArea);
    createDockWidgets();
    createConnets();
    createMenus();
    createToolButtons();
    readSettings();
    UserAuthManager::Instance()->load();
    sessionDockWidget->loadSessions();
    buttonsDockWidget->loadCommands();
    loadStyleSheet();
    QTimer::singleShot(SHOW_WINDOW_TIME_OUT, this, SLOT(loadWindowState()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    sessionDockWidget->saveSessions();
    buttonsDockWidget->saveCommands();
    UserAuthManager::Instance()->save();
    mdiArea->closeAllSubWindows();
    if (mdiArea->currentSubWindow()) {
        event->ignore();
    } else {
        writeSettings();
        event->accept();
    }
}

void MainWindow::newConnect()
{
    addSession(true);
}

void MainWindow::createConnets()
{
    connect(mdiArea, &QMdiArea::subWindowActivated,
            this, &MainWindow::updateStatus);

    connect(ui->actionQuickConnect, &QAction::triggered,
            this, &MainWindow::newConnect);
    connect(ui->actionConnect, &QAction::triggered,
            this, &MainWindow::reconnect);
    connect(ui->actionDisconnect, &QAction::triggered,
            this, &MainWindow::cancelconnect);
    connect(ui->actionDisplay, &QAction::triggered,
            this, &MainWindow::display);
    connect(ui->actionUndisplay, &QAction::triggered,
            this, &MainWindow::undisplay);

    connect(ui->actionKermitReceive, &QAction::triggered,
            [=](){ recvFile("Kermit"); });
    connect(ui->actionKermitSend, &QAction::triggered,
            [=](){ sendFile("Kermit"); });
    connect(ui->actionXModemReceive, &QAction::triggered,
            [=](){ recvFile("XMODEM"); });
    connect(ui->actionXModemSend, &QAction::triggered,
            [=](){ sendFile("XMODEM"); });
    connect(ui->actionYModemReceive, &QAction::triggered,
            [=](){ recvFile("YMODEM"); });
    connect(ui->actionYModemSend, &QAction::triggered,
            [=](){ sendFile("YMODEM"); });

    connect(ui->actionSave, &QAction::triggered,
            this, &MainWindow::save);
    connect(ui->actionPrint, &QAction::triggered,
            this, &MainWindow::print);
    connect(ui->actionExit, &QAction::triggered,
            this, &MainWindow::close);

    connect(ui->actionCopy, &QAction::triggered,
            this, &MainWindow::copy);
    connect(ui->actionCopyAll, &QAction::triggered,
            this, &MainWindow::copyAll);
    connect(ui->actionPaste, &QAction::triggered,
            this, &MainWindow::paste);
    connect(ui->actionFontIncrease, &QAction::triggered,
            this, &MainWindow::increaseFontSize);
    connect(ui->actionFontDecrease, &QAction::triggered,
            this, &MainWindow::decreaseFontSize);
    connect(ui->actionClearScrollBack, &QAction::triggered,
            this, &MainWindow::clearScrollback);

    connect(ui->actionTFtpStart, &QAction::triggered,
            this, &MainWindow::tftpServerStart);
    connect(ui->actionTFtpStop, &QAction::triggered,
            this, &MainWindow::tftpServerStop);

    connect(ui->actionLog, &QAction::toggled,
        this, &MainWindow::logToggle);
    connect(ui->actionEnglish, &QAction::triggered, this, [=](){
        setLanguage(ui->actionEnglish->text());
    });
    connect(ui->actionChinese, &QAction::triggered, this, [=](){
        setLanguage(ui->actionChinese->text());
    });
    connect(ui->actionWindowsVista, &QAction::triggered, this, [=](){
        setShowStyle(ui->actionWindowsVista->text());
    });
    connect(ui->actionWindows, &QAction::triggered, this, [=](){
        setShowStyle(ui->actionWindows->text());
    });
    connect(ui->actionFusion, &QAction::triggered, this, [=](){
        setShowStyle(ui->actionFusion->text());
    });

    connect(ui->actionTab, &QAction::triggered,
            this, &MainWindow::midViewMode);
    connect(ui->actionClose, &QAction::triggered,
            mdiArea, &QMdiArea::closeActiveSubWindow);
    connect(ui->actionCloseAll, &QAction::triggered,
            mdiArea, &QMdiArea::closeAllSubWindows);
    connect(ui->actionHorizontalTile, &QAction::triggered,
            this, &MainWindow::horizontalTileChileWindow);
    connect(ui->actionVerticalTile, &QAction::triggered,
            this, &MainWindow::verticalTileChileWindow);
    connect(ui->actionTitle, &QAction::triggered,
            this, &MainWindow::tileChildWindow);
    connect(ui->actionCascade, &QAction::triggered,
            this, &MainWindow::cascadeChildWindow);
    connect(ui->actionMaximize, &QAction::triggered,
            this, &MainWindow::maximizeChildWidnow);
    connect(ui->actionNext, &QAction::triggered,
            mdiArea, &QMdiArea::activateNextSubWindow);
    connect(ui->actionPrevious, &QAction::triggered,
            mdiArea, &QMdiArea::activatePreviousSubWindow);
    connect(ui->actionAbout, &QAction::triggered, []{
        AboutDialog dialog;
        dialog.exec();
    });

    connect(buttonsDockWidget, &ButtonsDockWidget::getCommand,
            this, &MainWindow::sendCommand);
    connect(commandDockWidget, &CommandDockWidget::getCommand,
            this, &MainWindow::sendCommand);
    connect(sessionDockWidget, &SessionDockWidget::onCreateShell,
            this, &MainWindow::createShell);
    connect(sessionDockWidget, &SessionDockWidget::onSessionSizeChanged,
            [=](QWidget* widget)
    {
        QMdiSubWindow* subWindow = activeSubWindow();
        if(subWindow && subWindow->widget() == widget)
        {
            Session::Ptr session = sessionDockWidget->findSession(subWindow->widget());
            if(session)
                session->activeWidget(widget);
        }
    });
    connect(sessionDockWidget, &SessionDockWidget::onCreateSession,
            [=]{
        addSession(false);
    });
    connect(sessionDockWidget, &SessionDockWidget::onSessionClose,
            [=]{
        QTimer::singleShot(SHOW_WINDOW_TIME_OUT, this, [=](){
            updateWindowState();
        });
    });

    ui->actionNext->setShortcuts(QKeySequence::NextChild);
    ui->actionPrevious->setShortcuts(QKeySequence::PreviousChild);

    connect(tftpServer_, &TFtpServer::statusText,
            this, &MainWindow::showStatusText);

    connect(this, &QWidget::windowTitleChanged,
            this, &MainWindow::windowTitleChanged);
}

void MainWindow::tileChildWindow()
{
    mdiArea->tileSubWindows();
    windowMode = Tile;
}

void MainWindow::horizontalTileChileWindow()
{
    QList<QMdiSubWindow *> widgets = mdiArea->subWindowList();
    if(widgets.isEmpty())
        return;

    QRect rect = mdiArea->viewport()->rect();
    int w = rect.width() / widgets.size();
    mdiArea->tileSubWindows();
    for(int i = 0, x = 0; i < widgets.size(); i++, x += w)
    {
        QWidget *widget = widgets.at(i);
        if(i == widgets.size() - 1)
            w = rect.width() - (widgets.size() - 1) * w;
        QRect newGeometry = QRect(QPoint(x, 0), QSize(w, rect.height()));
        widget->setGeometry(QStyle::visualRect(widget->layoutDirection(),
                                               rect, newGeometry));
    }
    windowMode = HorizontalTile;
}

void MainWindow::verticalTileChileWindow()
{
    QList<QMdiSubWindow *> widgets = mdiArea->subWindowList();
    if(widgets.isEmpty())
        return;

    QRect rect = mdiArea->viewport()->rect();
    int h = rect.height() / widgets.size();
    mdiArea->tileSubWindows();
    for(int i = 0, y = 0; i < widgets.size(); i++, y += h)
    {
        QWidget *widget = widgets.at(i);
        if(i == widgets.size() - 1)
            h = rect.height() - (widgets.size() - 1) * h;

        QRect newGeometry = QRect(QPoint(0, y), QSize(rect.width(), h));
        widget->setGeometry(QStyle::visualRect(widget->layoutDirection(),
                                               rect, newGeometry));
    }
    windowMode = VerticalTile;
}

void MainWindow::cascadeChildWindow()
{
    mdiArea->cascadeSubWindows();
    windowMode = Cascade;
}

void MainWindow::maximizeChildWidnow()
{
    QMdiSubWindow* window = activeSubWindow();
    if(window)
        window->showMaximized();
    windowMode = Max;
}

void MainWindow::createDockWidgets()
{
    addDockWidget(Qt::LeftDockWidgetArea, sessionDockWidget);
    addDockWidget(Qt::BottomDockWidgetArea, buttonsDockWidget);
    splitDockWidget(buttonsDockWidget, commandDockWidget, Qt::Vertical);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
}

void MainWindow::createMenus()
{
    ui->menuView->addAction(ui->toolBar->toggleViewAction());
    statusBarAction = ui->menuView->addAction(tr("Status Bar"));
    statusBarAction->setCheckable(true);
    statusBarAction->setMenuRole(QAction::NoRole);
    statusBarAction->setChecked(ui->statusBar->isVisible());
    connect(statusBarAction, SIGNAL(triggered(bool)), ui->statusBar, SLOT(setVisible(bool)));

    ui->menuView->addSeparator();
    QAction* sessionAction = sessionDockWidget->toggleViewAction();
    QAction* commandAction = commandDockWidget->toggleViewAction();
    sessionAction->setShortcut(QKeySequence("Ctrl+B"));
    commandAction->setShortcut(QKeySequence("Ctrl+M"));

    ui->menuView->addAction(sessionAction);
    ui->menuView->addAction(buttonsDockWidget->toggleViewAction());
    ui->menuView->addAction(commandAction);
    windowGroup->addAction(ui->actionHorizontalTile);
    windowGroup->addAction(ui->actionVerticalTile);
    windowGroup->addAction(ui->actionTitle);
    windowGroup->addAction(ui->actionCascade);
    windowGroup->addAction(ui->actionMaximize);
    ui->actionLog->setChecked(isLog);
    languageGroup->addAction(ui->actionEnglish);
    languageGroup->addAction(ui->actionChinese);
    showstyleGroup->addAction(ui->actionWindowsVista);
    showstyleGroup->addAction(ui->actionWindows);
    showstyleGroup->addAction(ui->actionFusion);
    if(language == ui->actionEnglish->text())
        ui->actionEnglish->setChecked(true);
    else if(language == ui->actionChinese->text())
        ui->actionChinese->setChecked(true);

    if(showStyle == ui->actionWindowsVista->text())
        ui->actionWindowsVista->setChecked(true);
    else if(showStyle == ui->actionWindows->text())
        ui->actionWindows->setChecked(true);
    else if(showStyle == ui->actionFusion->text())
        ui->actionFusion->setChecked(true);

    createHighLighterMenu();
}

void MainWindow::createToolButtons()
{
    comboColor = new QComboBox(ui->toolBar);
    comboPalette = new QComboBox(ui->toolBar);
    comboFont = new QFontComboBox(ui->toolBar);
    comboSize = new QComboBox(ui->toolBar);
    buttonCodec = new QToolButton(ui->statusBar);
    comboFont->setFontFilters(QFontComboBox::MonospacedFonts);
    comboFont->setEditable(false);
    ui->toolBar->addWidget(comboColor);
    ui->toolBar->addWidget(comboPalette);
    ui->toolBar->addSeparator();
    ui->toolBar->addWidget(comboFont);
    ui->toolBar->addWidget(comboSize);
    ui->statusBar->addPermanentWidget(buttonCodec);

    QStringList colorNames = ConsoleColorManager::Instance()->colorNames();
    comboColor->addItems(colorNames);

    ConsolePaletteManager* manager = ConsolePaletteManager::Instance();
    for(int i = 0; i < manager->size(); i++)
        comboPalette->addItem(manager->palette(i)->name());

    const QList<int> standardSizes = QFontDatabase::standardSizes();
    foreach (int size, standardSizes)
        comboSize->addItem(QString::number(size));

    comboColor->setToolTip(tr("Background and foreground of Console"));
    comboPalette->setToolTip(tr("Paletee of Console"));
    comboFont->setToolTip(tr("Font name of Console"));
    comboSize->setToolTip(tr("Font size of Console"));
    buttonCodec->setToolTip(tr("Codec of Console"));
    buttonCodec->setText("UTF-8");
    buttonCodec->setAutoRaise(true);
    QColor color = palette().highlight().color().lighter(210);
    QString colorText = QString("#%1%2%3")
        .arg(color.red(), 2, 16, QChar('0'))
        .arg(color.green(), 2, 16, QChar('0'))
        .arg(color.blue(), 2, 16, QChar('0'));
    buttonCodec->setStyleSheet(QString("QToolButton{margin-right:5px;border-width:1px;border-radius:2px;}"
                               "QToolButton:hover {background:%1;}"
                               "QToolButton:pressed{background:%1;}").arg(colorText)
                               );
    connect(comboColor, SIGNAL(activated(int)), this, SLOT(setColorIndex(int)));
    connect(comboPalette, SIGNAL(activated(QString)), this, SLOT(setPaletteName(QString)));
    connect(comboFont, SIGNAL(activated(QString)), this, SLOT(setFontName(QString)));
    connect(comboSize, SIGNAL(activated(QString)), this, SLOT(setFontSize(QString)));
    connect(buttonCodec, SIGNAL(clicked()), this, SLOT(selectCodec()));
}

void MainWindow::addSession(bool isCreateSheel)
{
    ConnectDialog dlg;
    dlg.setType(connectType);
    if(dlg.exec() == QDialog::Accepted)
    {
        Session::Ptr session;
        connectType = dlg.type();
        if(connectType == ConnectType::SSH)
        {
            SSHSettings settings = dlg.sshSettings();
            session = Session::Ptr(new SshSession(settings.name(), settings));
        }
        else if(connectType == ConnectType::Telnet)
        {
            TelnetSettings settings = dlg.telnetSettings();
            session = Session::Ptr(new TelnetSession(settings.name(), settings));
        }
        else if(connectType == ConnectType::Serial)
        {
            SerialSettings settings = dlg.serialSettings();
            session = Session::Ptr(new SerialSession(settings.port, settings));
        }
        else if(connectType == ConnectType::Local)
        {
            LocalShellSettings settings = dlg.localShellSettings();
            session = Session::Ptr(new LocalShellSession(settings.name(), settings));
        }
        else if(connectType == ConnectType::WSL)
        {
            WSLSettings settings = dlg.wslSettings();
            session = Session::Ptr(new WSLSession(settings.name(), settings));
        }
        if(session)
        {
            if(isCreateSheel)
                createShell(session);
            sessionDockWidget->addSession(session);
        }
    }
}

void MainWindow::midViewMode()
{
    if(mdiArea->viewMode() == QMdiArea::SubWindowView){
        mdiArea->setViewMode(QMdiArea::TabbedView);
        windowMode = Max;
        QTabBar* tabBar = dynamic_cast<QTabBar*>(mdiArea->findChild<QTabBar *>());
        if(tabBar)
            tabBar->setExpanding(false);
    }
    else
        mdiArea->setViewMode(QMdiArea::SubWindowView);
    ui->actionTab->setChecked(mdiArea->viewMode() == QMdiArea::TabbedView);
}

void MainWindow::updateWindowState()
{
    if(windowMode == Tile)
    {
        ui->actionTitle->setChecked(true);
        mdiArea->tileSubWindows();
    }
    else if(windowMode == HorizontalTile)
    {
        ui->actionHorizontalTile->setChecked(true);
        horizontalTileChileWindow();
    }
    else if(windowMode == VerticalTile)
    {
        ui->actionVerticalTile->setChecked(true);
        verticalTileChileWindow();
    }
    else if(windowMode == Cascade)
    {
        ui->actionCascade->setChecked(true);
        mdiArea->cascadeSubWindows();
    }
    else if(windowMode == Max)
    {
        ui->actionMaximize->setChecked(true);
        maximizeChildWidnow();
    }
}

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::applicationName(),
                       QCoreApplication::applicationVersion());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    const QByteArray windowState = settings.value("windowState", QByteArray()).toByteArray();

    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }

    if(!windowState.isEmpty())
        restoreState(windowState);

    uint viewMode = settings.value("viewMode", QMdiArea::SubWindowView).toUInt();
    if(viewMode == 0)
        mdiArea->setViewMode(QMdiArea::SubWindowView);
    else
    {
        mdiArea->setViewMode(QMdiArea::TabbedView);
        QTabBar* tabBar = dynamic_cast<QTabBar*>(mdiArea->findChild<QTabBar *>());
        if(tabBar)
            tabBar->setExpanding(false);
    }
    ui->actionTab->setChecked(mdiArea->viewMode() == QMdiArea::TabbedView);

    bool isVisible = settings.value("statusBarIsVisable", true).toBool();
    ui->statusBar->setVisible(isVisible);

    connectType = static_cast<ConnectType>(settings.value("connectType", connectType).toUInt());
    tftpRootPath = settings.value("tftpRootPath", tftpRootPath).toString();
    isLog = settings.value("isLog", false).toBool();
    ui->actionLog->setChecked(isLog);
}

void MainWindow::loadWindowState()
{
    QSettings settings(QCoreApplication::applicationName(),
                       QCoreApplication::applicationVersion());
    windowMode = static_cast<WindowMode>(settings.value("windowMode", windowMode).toUInt());
    updateWindowState();
}

void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::applicationName(),
                       QCoreApplication::applicationVersion());
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("viewMode", mdiArea->viewMode());
    settings.setValue("windowMode", windowMode);
    settings.setValue("statusBarIsVisable", ui->statusBar->isVisible());
    settings.setValue("connectType", connectType);
    settings.setValue("tftpRootPath", tftpRootPath);
    settings.setValue("isLog", isLog);
}

QMdiSubWindow* MainWindow::activeSubWindow()
{
    QMdiSubWindow* subWindow = mdiArea->activeSubWindow();
    if(!subWindow)
    {
        QList<QMdiSubWindow *> subWindows = mdiArea->subWindowList();
        if(!subWindows.isEmpty())
            subWindow = subWindows.last();
    }
    return subWindow;
}

Session::Ptr MainWindow::activeSession()
{
    QMdiSubWindow* subWindow = activeSubWindow();
    Session::Ptr session;
    if(subWindow)
        session = sessionDockWidget->findSession(subWindow->widget());
    return session;
}

void MainWindow::updateStatus(QMdiSubWindow *subWindow)
{
    if(!subWindow)
        setWindowTitle(QApplication::applicationName());
    else
    {
        Session::Ptr session = sessionDockWidget->findSession(subWindow->widget());
        if(session)
        {
            comboColor->setCurrentIndex(session->colorIndex());
            comboPalette->setCurrentText(session->paletteName());
            buttonCodec->setText(session->codecName());
            comboFont->setCurrentText(session->fontName());
            comboSize->setCurrentText(QString::number(session->fontSize()));
            udpateHighLighterMenuStatus(session->hightLighter());
            session->activeWidget(subWindow->widget());
            setDiaplay(session->isDisplay(subWindow->widget()));
            setConnect(session->isConnected(subWindow->widget()));
            setSubTitle(session->subTitle());
        }
    }
}

void MainWindow::updateFontSize(int fontSize)
{
    comboSize->setCurrentText(QString::number(fontSize));
}

void  MainWindow::connectStatus(QWidget* widget, bool on)
{
    QMdiSubWindow* subWindow = activeSubWindow();
    if(subWindow)
    {
        if(subWindow->widget() == widget)
            setConnect(on);
    }
}

void MainWindow::createShell(Session::Ptr & session)
{
    disconnect(session.get(), &Session::fontSizeChanged, this, &MainWindow::updateFontSize);
    disconnect(session.get(), &Session::highLighterChanged, this, &MainWindow::udpateHighLighter);
    disconnect(session.get(), &Session::windowStateChanged, this, &MainWindow::subWindowStateChanged);
    disconnect(session.get(), &Session::onSubTitle, this, &MainWindow::setSubTitle);
    disconnect(session.get(), &Session::onConnectStatus, this, &MainWindow::connectStatus);
    disconnect(session.get(), &Session::onCommand, commandDockWidget, &CommandDockWidget::addToHistory);
    disconnect(session.get(), &Session::onCreateShell, sessionDockWidget, &SessionDockWidget::createShell);

    connect(session.get(), &Session::fontSizeChanged, this, &MainWindow::updateFontSize);
    connect(session.get(), &Session::highLighterChanged, this, &MainWindow::udpateHighLighter);
    connect(session.get(), &Session::windowStateChanged, this, &MainWindow::subWindowStateChanged);
    connect(session.get(), &Session::onSubTitle, this, &MainWindow::setSubTitle);
    connect(session.get(), &Session::onConnectStatus, this, &MainWindow::connectStatus);
    connect(session.get(), &Session::onCommand, commandDockWidget, &CommandDockWidget::addToHistory);
    connect(session.get(), &Session::onCreateShell, sessionDockWidget, &SessionDockWidget::createShell);

    if(session->createShell(mdiArea, isLog))
    {
        session->runShell();
        updateStatus(activeSubWindow());
        updateWindowState();
    }
}

void MainWindow::reconnect()
{
    QMdiSubWindow* subWindow = activeSubWindow();
    if(subWindow)
    {
        Session::Ptr session = sessionDockWidget->findSession(subWindow->widget());
        if(session)
        {
            session->reconnect(subWindow->widget());
            setConnect(true);
        }
    }
}

void MainWindow::cancelconnect()
{
    QMdiSubWindow* subWindow = activeSubWindow();
    if(subWindow)
    {
        Session::Ptr session = sessionDockWidget->findSession(subWindow->widget());
        if(session)
        {
            session->disconnect(subWindow->widget());
            setConnect(false);
        }
    }
}

void MainWindow::display()
{
    QMdiSubWindow* subWindow = activeSubWindow();
    if(subWindow)
    {
        Session::Ptr session = sessionDockWidget->findSession(subWindow->widget());
        if(session)
        {
            session->display(subWindow->widget());
            setDiaplay(true);
        }
    }
}

void MainWindow::undisplay()
{
    QMdiSubWindow* subWindow = activeSubWindow();
    if(subWindow)
    {
        Session::Ptr session = sessionDockWidget->findSession(subWindow->widget());
        if(session)
        {
            session->undisplay(subWindow->widget());
            setDiaplay(false);
        }
    }
}

void MainWindow::setDiaplay(bool enable)
{
    ui->actionDisplay->setVisible(!enable);
    ui->actionUndisplay->setVisible(enable);
}

void MainWindow::setConnect(bool enable)
{
    ui->actionConnect->setVisible(!enable);
    ui->actionDisconnect->setVisible(enable);
}

void MainWindow::sendFile(QString const& protocol)
{
    QMdiSubWindow* subWindow = activeSubWindow();
    if(subWindow)
    {
        Session::Ptr session = sessionDockWidget->findSession(subWindow->widget());
        if(session)
            session->sendFile(subWindow->widget(), protocol);
    }
}

void MainWindow::recvFile(QString const& protocol)
{
    QMdiSubWindow* subWindow = activeSubWindow();
    if(subWindow)
    {
        Session::Ptr session = sessionDockWidget->findSession(subWindow->widget());
        if(session)
            session->recvFile(subWindow->widget(), protocol);
    }
}

void MainWindow::save()
{
    QMdiSubWindow* subWindow = activeSubWindow();
    if(subWindow)
    {
        Session::Ptr session = sessionDockWidget->findSession(subWindow->widget());
        if(session)
            session->save(subWindow->widget());
    }
}

void MainWindow::print()
{
    QMdiSubWindow* subWindow = activeSubWindow();
    if(subWindow)
    {
        Session::Ptr session = sessionDockWidget->findSession(subWindow->widget());
        if(session)
            session->print(subWindow->widget());
    }
}

void MainWindow::copy()
{
    QMdiSubWindow* subWindow = activeSubWindow();
    if(subWindow)
    {
        Session::Ptr session = sessionDockWidget->findSession(subWindow->widget());
        if(session)
            session->copy(subWindow->widget());
    }
}

void MainWindow::copyAll()
{
    QMdiSubWindow* subWindow = activeSubWindow();
    if(subWindow)
    {
        Session::Ptr session = sessionDockWidget->findSession(subWindow->widget());
        if(session)
            session->copyAll(subWindow->widget());
    }
}

void MainWindow::udpateHighLighter(QString const& lighter)
{
    Session::Ptr session = activeSession();
    if(session)
    {
        session->setHightLighter(lighter);
        udpateHighLighterMenuStatus(lighter);
    }
}

void MainWindow::selectCodec()
{
    QList<ConsoleCodec> const& codecs = ConsoleCodecManager::Instance()->codecs();
    QString name = ConsoleCodecManager::Instance()->codecToName(buttonCodec->text());
    QMenu menu;

    foreach(auto codec, codecs)
    {
        QAction *action = menu.addAction(codec.name, this, [=](){
            Session::Ptr session = activeSession();
            if(session)
            {
                session->setCodecName(codec.codec);
                buttonCodec->setText(codec.codec);
            }
        });
        if(action->text() == name)
        {
            action->setCheckable(true);
            action->setChecked(true);
        }
    }

    QPoint pos = buttonCodec->mapToGlobal(buttonCodec->rect().topLeft());
    menu.exec(pos);
}

void MainWindow::setFontName(QString const& name)
{
    Session::Ptr session = activeSession();
    if(session)
        session->setFontName(name);
}

void MainWindow::setColorIndex(int index)
{
    Session::Ptr session = activeSession();
    if(session)
        session->setColorIndex(index);
}

void MainWindow::setPaletteName(QString const& name)
{
    Session::Ptr session = activeSession();
    if(session)
        session->updatePaletteName(name);
}

void MainWindow::setFontSize(QString const& fontSize)
{
    Session::Ptr session = activeSession();
    if(session)
        session->setFontSize(fontSize.toInt());
}

void MainWindow::increaseFontSize()
{
    QMdiSubWindow* subWindow = activeSubWindow();
    if(subWindow)
    {
        Session::Ptr session = sessionDockWidget->findSession(subWindow->widget());
        if(session)
            session->increaseFontSize(subWindow->widget());
    }
}

void MainWindow::decreaseFontSize()
{
    QMdiSubWindow* subWindow = activeSubWindow();
    if(subWindow)
    {
        Session::Ptr session = sessionDockWidget->findSession(subWindow->widget());
        if(session)
            session->decreaseFontSize(subWindow->widget());
    }
}

void MainWindow::clearScrollback()
{
    QMdiSubWindow* subWindow = activeSubWindow();
    if(subWindow)
    {
        Session::Ptr session = sessionDockWidget->findSession(subWindow->widget());
        if(session)
            session->clearScrollback(subWindow->widget());
    }
}

void MainWindow::paste()
{
    QMdiSubWindow* subWindow = activeSubWindow();
    if(subWindow)
    {
        Session::Ptr session = sessionDockWidget->findSession(subWindow->widget());
        if(session)
            session->paste(subWindow->widget());
    }
}

void MainWindow::sendCommand(QString const& command)
{
    QMdiSubWindow* subWindow = activeSubWindow();
    if(subWindow)
    {
        Session::Ptr session = sessionDockWidget->findSession(subWindow->widget());
        if(session)
            session->sendCommand(subWindow->widget(), command);
    }
}

void MainWindow::logToggle(bool isChecked)
{
    isLog = isChecked;
}

void MainWindow::createHighLighterMenu()
{
    for(int i = 0; i < highLighterManager->size(); i++)
    {
        HighLighterManager::HighLighter const& lighter = highLighterManager->highLighter(i);
        QAction *action = ui->menuSyntaxHighlighting->addAction(lighter.showText(),
                                                                this, SLOT(setHighLighter()));
        action->setData(lighter.name);
        hightlightGroup->addAction(action);
        action->setCheckable(true);
    }
}

void MainWindow::udpateHighLighterMenuStatus(QString const& lighter)
{
    QList<QAction*> actions = ui->menuSyntaxHighlighting->actions();
    foreach(QAction* action, actions)
    {
        if(action->data().toString() == lighter)
        {
            action->setChecked(true);
            break;
        }
    }
}

void MainWindow::setHighLighter()
{
    QAction* action = (QAction*)sender();
    Session::Ptr session = activeSession();
    if(session && action)
        session->setHightLighter(action->data().toString());
}

void MainWindow::subWindowStateChanged(Qt::WindowStates oldState, Qt::WindowStates newState)
{
    Q_UNUSED(oldState)
    if(newState & Qt::WindowMaximized)
    {
        windowMode = Max;
        ui->actionMaximize->setChecked(true);
    }
}

void MainWindow::windowTitleChanged(const QString &title)
{
    Q_UNUSED(title)
    QString newTitle;
    QMdiSubWindow* subWindow = activeSubWindow();

    if(subWindow)
    {
        Session::Ptr session = sessionDockWidget->findSession(subWindow->widget());
        if(session)
            newTitle = session->subTitle();
    }
    setSubTitle(newTitle);
}

void MainWindow::setSubTitle(QString const& title)
{
    if(title.isEmpty())
        setWindowTitle(QApplication::applicationName());
    else
        setWindowTitle(QString("%1 - %2")
                   .arg(QApplication::applicationName(), title));
}

void MainWindow::showStatusText(QString const& text)
{
    ui->statusBar->showMessage(text, 2000);//2s
}

void MainWindow::tftpServerStart()
{
    QString filePath = QFileDialog::getExistingDirectory(
                this, tr("Select TFTP Path"), tftpRootPath);

    if(filePath.isEmpty())
        return;

    tftpRootPath = filePath;
    tftpServer_->setFilePath(tftpRootPath);
    tftpServer_->start();
    ui->actionTFtpStart->setText(QString(tr("Start server on %1 ")).arg(tftpRootPath));
    ui->actionTFtpStop->setText(QString(tr("Stop server from %1 ")).arg(tftpRootPath));
    ui->actionTFtpStart->setEnabled(false);
    ui->actionTFtpStop->setEnabled(true);
}

void MainWindow::tftpServerStop()
{
    tftpServer_->stop();
    ui->actionTFtpStop->setEnabled(false);
    ui->actionTFtpStart->setEnabled(true);
}

void MainWindow::loadStyleSheet()
{
    setStyleSheet("QMainWindow::separator{width:1px;height:1px;}"
                  "QTabBar::tab{"
                  "padding-left: 6px;"
                  "padding-top: 4px;"
                  "padding-right: 4px;"
                  "padding-bottom: 3px;"
                  "background: #F3F3F3;"
                  "border: 1px solid #E5E5E5;"
                  "border-top-left-radius: 8px;"
                  "border-top-right-radius: 8px;}"
                  "QTabBar::tab:hover{"
                  "background: #EDEDED;}"
                  "QTabBar::tab:selected{"
                  "border-top: 1px solid #007DE0;"//#1BC6F4 90C8F6 3F48BF #007DE0
                  "margin-left: -6px;"
                  "margin-right: -4px;"
                  "background: #FAFAFA;}"
                  "QTabBar::tab:first:selected {"
                  "margin-left: 0px;}"
                  "QTabBar::tab:last:selected {"
                  "margin-right: 0px;}"
                  "QTabBar::close-button {"
                  "image: url(:/image/Term/close.png);}"
                  "QTabBar::close-button:hover {"
                  "image: url(:/image/Term/close-hover.png);}");
}

void MainWindow::setShowStyle(QString const& style)
{
    showStyle = style;
    QApplication::setStyle(QStyleFactory::create(showStyle));
    SaveSettings();
}

void MainWindow::setLanguage(QString const& lang)
{
    language = lang;
    InstallTranstoirs(true);
    retranslateUi();
    sessionDockWidget->retranslateUi();
    buttonsDockWidget->retranslateUi();
    commandDockWidget->retranslateUi();
    SaveSettings();
}

void MainWindow::retranslateUi()
{
    ui->retranslateUi(this);
    statusBarAction->setText(tr("Status Bar"));

    int currentindex = comboColor->currentIndex();
    comboColor->clear();
    comboColor->addItems(ConsoleColorManager::Instance()->colorNames());
    comboColor->setCurrentIndex(currentindex);


    QList<QAction *> actions = ui->menuSyntaxHighlighting->actions();
    for(int i = 0; i < highLighterManager->size() && i < actions.size(); i++)
    {
        HighLighterManager::HighLighter const& lighter = highLighterManager->highLighter(i);
        actions[i]->setText(lighter.showText());
    }
}
