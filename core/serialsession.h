#ifndef SERIALSESSION_H
#define SERIALSESSION_H

#include "basesession.h"
#include "serialsettings.h"

class SerialPortWidget;
class SerialSession : public Session
{
    Q_OBJECT
public:
    SerialSession(QString const& name);
    SerialSession(QString const& name, SerialSettings const& settings);

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
    void updateCodecName(QString const& codecName) override;
    void updateFontName(QString const& fontName) override;
    void updateColorIndex(int index) override;
    void updatePaletteName(QString const&) override;
    void updateFontSize(int fontSize) override;
    void increaseFontSize(QWidget *widget) override;
    void decreaseFontSize(QWidget *widget) override;
    void clearScrollback(QWidget *widget) override;
    void sendFile(QWidget *widget, QString const& protocol) override;
    void recvFile(QWidget *widget, QString const& protocol) override;

    void setObject(QJsonObject const& obj) override;
    QJsonObject object() const override;

    SerialSettings settings() const { return settings_; }
private slots:
    void setHighLighter();
private:
    void updateTitle(QString const& name) override;
private:
    SerialSettings settings_;
    SerialPortWidget* widget_ = nullptr;
};

#endif // SERIALSESSION_H
