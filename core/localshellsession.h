#ifndef LOCALSHELLSESSION_H
#define LOCALSHELLSESSION_H

#include "basesession.h"
#include "localshellsettings.h"

class LocalShellWidget;
class LocalShellSession : public Session
{
    Q_OBJECT
public:
    LocalShellSession(QString const& name);
    LocalShellSession(QString const& name, LocalShellSettings const& settings);

    QString type() const override;
    QIcon icon() override;
    void edit() override;
    bool createShell(QMdiArea *midArea, bool isLog) override;
    bool runShell() override;
    void closeSheel(QWidget *widget) override;
    bool isThisWidget(QWidget *widget) override;
    int  shellSize() override;

    void sendCommand(QWidget *widget, QString const& command) override;
    bool isConnected(QWidget *widget) override;
    void reconnect(QWidget *widget) override;
    void disconnect(QWidget *widget) override;
    bool isDisplay(QWidget *widget) const override;
    void display(QWidget *widget) override;
    void undisplay(QWidget *widget) override;
    void save(QWidget *widget) override;
    void print(QWidget *widget) override;
    void copy(QWidget *widget) override;
    void copyAll(QWidget *widget) override;
    void paste(QWidget *widget) override;
    void updateHightLighter(QString const& hightLighter) override;
    void updateCodecName(QString const& codecName) override;
    void updateFontName(QString const& fontName) override;
    void updateColorIndex(int index) override;
    void updatePaletteName(QString const& paletteName) override;
    void updateFontSize(int fontSize) override;
    void increaseFontSize(QWidget *widget) override;
    void decreaseFontSize(QWidget *widget) override;
    void clearScrollback(QWidget *widget) override;

    void setObject(QJsonObject const& obj) override;
    QJsonObject object() const override;

    LocalShellSettings settings() const { return settings_; }
private slots:
    void setHighLighter();
protected:
    void updateTitle(QString const& name) override;
    QMenu* createSystemMenu(QMdiSubWindow *parent, QWidget *widget) override;
private:
    LocalShellSettings settings_;
    QList<LocalShellWidget*> widgets_;
    int index = 1;
};

#endif // LOCALSHELLSESSION_H
