#ifndef SSHSESSION_H
#define SSHSESSION_H

#include "session.h"
#include "sshsettings.h"

class SShWidget;
class SshSession : public Session
{
    Q_OBJECT
public:
    SshSession(QString const& name);
    SshSession(QString const& name, SSHSettings const& settings);

    QString type() const override;
    QIcon icon() override;
    void edit() override;
    bool createShell(QMdiArea *midArea, bool isLog) override;
    bool runShell() override;
    void closeSheel(QWidget *widget) override;
    bool isThisWidget(QWidget *widget) override;
    int  shellSize() override;

    void sendCommand(QWidget *widget, QString const& command) override;
    void disconnect(QWidget *widget) override;
    void save(QWidget *widget) override;
    void print(QWidget *widget) override;
    void copy(QWidget *widget) override;
    void copyAll(QWidget *widget) override;
    void paste(QWidget *widget) override;
    void updateHightLighter(QString const& hightLighter) override;
    void updateFontName(QString const& fontName) override;
    void updateColorIndex(int index) override;
    void updatePaletteName(QString const& paletteName) override;
    void updateFontSize(int fontSize) override;
    void increaseFontSize(QWidget *widget) override;
    void decreaseFontSize(QWidget *widget) override;
    void clearScrollback(QWidget *widget) override;

    void setObject(QJsonObject const& obj) override;
    QJsonObject object() const override;

    SSHSettings settings() const { return settings_; }
private slots:
    void setHighLighter();
private:
    void updateTitle(QString const& name) override;
private:
    SSHSettings settings_;
    QList<SShWidget*> widgets_;
    int index = 1;
};

#endif // SSHSESSION_H
