#ifndef BASESESSION_H
#define BASESESSION_H
#include <QString>

#include <memory>
#include <QJsonObject>
#include <QList>
#include <QIcon>

class QMdiArea;
class QWidget;
class QMdiSubWindow;
class QMenu;
class Session : public QObject
{
    Q_OBJECT
public:
    Session(QString const& name);
    virtual ~Session(){}
    using Ptr = std::shared_ptr<Session>;

    QString name() const { return name_; }
    void setName(QString const& name);

    QString hightLighter() const { return hightLighter_; }
    void setHightLighter(QString const& hightLighter);

    QString codecName() const { return codecName_; }
    void setCodecName(QString const& codecName);

    QString fontName() const { return fontName_; }
    void setFontName(QString const& fontName);

    int fontSize() const { return fontSize_; }
    void setFontSize(int fontSize);

    int colorIndex() const { return colorIndex_; }
    void setColorIndex(int index);

    QString paletteName() const { return paletteName_; }
    void setPaletteName(QString const& name);

    QString subTitle() const { return subTitle_; }
    void setSubTitle(QString const& title) { subTitle_ = title; }

    virtual QString type() const = 0;
    virtual QIcon icon() = 0;
    virtual void edit() = 0;
    virtual bool createShell(QMdiArea *midArea, bool isLog) = 0;
    virtual bool runShell() = 0;
    virtual void closeSheel(QWidget *widget) = 0;
    virtual int  shellSize() = 0;
    virtual bool isThisWidget(QWidget *widget) = 0;

    virtual void sendCommand(QWidget *widget, QString const& command) = 0;
    virtual bool isConnected(QWidget *widget) = 0;
    virtual void reconnect(QWidget *widget) = 0;
    virtual void disconnect(QWidget *widget) = 0;
    virtual bool isDisplay(QWidget *widget) const = 0;
    virtual void display(QWidget *widget) = 0;
    virtual void undisplay(QWidget *widget) = 0;
    virtual void save(QWidget *widget) = 0;
    virtual void print(QWidget *widget) = 0;
    virtual void copy(QWidget *widget) = 0;
    virtual void copyAll(QWidget *widget) = 0;
    virtual void paste(QWidget *widget) = 0;
    virtual void updateHightLighter(QString const& hightLighter) = 0;
    virtual void updateCodecName(QString const& codecName) = 0;
    virtual void updateFontName(QString const& fontName) = 0;
    virtual void updateFontSize(int fontSize) = 0;
    virtual void updateColorIndex(int index) = 0;
    virtual void updatePaletteName(QString const& colorName) = 0;
    virtual void increaseFontSize(QWidget *widget) = 0;
    virtual void decreaseFontSize(QWidget *widget) = 0;
    virtual void clearScrollback(QWidget *widget) = 0;
    virtual void activeWidget(QWidget*) {}

    virtual void sendFile(QWidget *widget, QString const& protocol);
    virtual void recvFile(QWidget *widget, QString const& protocol);
    virtual void setObject(QJsonObject const& obj) = 0;
    virtual QJsonObject object() const = 0;
signals:
    void onClose(QWidget *widget);
    void onCreateShell(QWidget *widget);
    void onSizeChanged(QWidget *widget);
    void onCommand(QString const& command);
    void onSubTitle(QString const& title);
    void fontSizeChanged(int fonstSize);
    void highLighterChanged(QString const& highLight);
    void windowStateChanged(Qt::WindowStates oldState, Qt::WindowStates newState);
protected:
    virtual void updateTitle(QString const& name) = 0;
    virtual QMenu* createSystemMenu(QMdiSubWindow *parent, QWidget *widget);
private:
    QString name_;
    QString hightLighter_;
    QString codecName_;
    QString fontName_;
    int  colorIndex_;
    QString paletteName_;
    QString subTitle_;
    int fontSize_;
};

class SessionManager
{
public:
    SessionManager();
    using Ptr = std::shared_ptr<SessionManager>;

    int size() const;
    Session::Ptr session(int index);
    int shellSize(int index) const;
    Session::Ptr findSession(QWidget *widget);

    void addSession(Session::Ptr const& session);
    void removeSession(Session::Ptr const& session);
    void upSession(int index);
    void downSession(int index);

    bool save(QString const& fileName);
    bool load(QString const& fileName);
private:
    Session::Ptr createSession(QJsonObject const& obj);
private:
    QList<Session::Ptr> sessions_;
    QList<int> shellSizes_;
};

#endif // BASESESSION_H
