#ifndef TELNETWIDGET_H
#define TELNETWIDGET_H

#include "child.h"

class QtTelnet;
class TelnetSettings;
class QAbstractSocket;
class TelnetWidget : public Child
{
    Q_OBJECT
public:
    explicit TelnetWidget(bool isLog, QWidget *parent = nullptr);

    bool runShell(TelnetSettings const& settings);

    QString errorString();
    void setErrorText(QString const& text);

    bool isConnected() const { return isConnected_; }
    void reconnect(TelnetSettings const& settings);
    void disconnect();
protected:
    void resizeEvent(QResizeEvent *event) override;
    Console* createConsole() override;
    void writeToShell(QByteArray const& data) override;
    void onPullData() override;
private slots:
    void onData(QString const& data);
private:
    QtTelnet* telnet;
     bool isConnected_ = false;
    QStringList texts;
    LogFile::SharedPtr beforeLogfile_;

};

#endif // TELNETWIDGET_H
