#ifndef WSLWIDGET_H
#define WSLWIDGET_H

#include "child.h"

class LocalShell;
class WSLSettings;
class WSLWidget : public Child
{
    Q_OBJECT
public:
    WSLWidget(bool isLog, QWidget *parent = nullptr);

    bool runShell(WSLSettings const& settings);

    bool isConnected() const { return isConnected_; }
    void reconnect(WSLSettings const& settings);
    void disconnect();

protected:
    Console* createConsole() override;
    void writeToShell(QByteArray const& data) override;
    void onPullData() override;
private slots:
    void onData(QByteArray const& data);
private:
    LocalShell* shell;
    bool isConnected_ = false;
    QByteArrayList datas;
    LogFile::SharedPtr beforeLogfile_;
};

#endif // WSLWIDGET_H
