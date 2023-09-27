#ifndef LOCALSHELLWIDGET_H
#define LOCALSHELLWIDGET_H

#include "child.h"

class QMenu;
class Console;
class LocalShell;
class LocalShellSettings;
class CommandThread;
class LocalShellWidget : public Child
{
    Q_OBJECT
public:
    explicit LocalShellWidget(bool isLog, QWidget *parent = nullptr);

    bool runShell(LocalShellSettings const& settings);
    bool runCmdShell(LocalShellSettings const& settings);
    bool runPowerShell(LocalShellSettings const& settings);

    bool isConnected() const { return isConnected_; }
    void reconnect(LocalShellSettings const& settings);
    void disconnect();

protected:
    Console* createConsole() override;
    void writeToShell(QByteArray const& data) override;
    void onPullData() override;
private slots:
    void onData(QByteArray const& data);
    void writeData(QByteArray const&data);
    void setCommand(QString const& command);
private:
    LocalShell* shell;
    QStringList texts;
    LogFile::SharedPtr beforeLogfile_;
    bool isConnected_ = false;
    QByteArray testData_;
    QString command_;
};

#endif // LOCALSHELLWIDGET_H
