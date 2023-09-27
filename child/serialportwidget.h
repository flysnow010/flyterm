#ifndef SERIALPORTWIDGET_H
#define SERIALPORTWIDGET_H

#include "child.h"

class QMenu;
class Console;
class QSerialPort;
class SerialSettings;
class SerialPortWidget : public Child
{
    Q_OBJECT
public:
    explicit SerialPortWidget(bool isLog, QWidget *parent = nullptr);
    ~SerialPortWidget();

    bool runShell(SerialSettings const& settings);

    QString errorString();
    void setErrorText(QString const& text);

    bool isConnected() const;
    void reconnect();
    void disconnect();
public slots:
    void recvFileByKermit();
    void sendFileByKermit();
    void recvFileByXModem();
    void sendFileByXModem();
    void recvFileByYModem();
    void sendFileByYModem();
protected:
    Console* createConsole() override;
    void writeToShell(QByteArray const& data) override;
    void onPullData() override;
    void onContextMenu(QMenu &contextMenu) override;
    void onExecCommand(QString const& command) override;
    void onExpandCommand(QString const& command) override;
private slots:
    void onData();
private:
    void sendFileByKermit(QString const& fileName);
    void recvFileByKermit(QString const& fileName);
    void sendFileByXYModem(QString const& fileName, bool isYModem);
    void recvFileByXYModem(QString const& fileName, bool isYModem);
private:
    QSerialPort* serial;
    QByteArrayList datas;
    LogFile::SharedPtr beforeLogfile_;
    QString filePath;
};
#endif // SERIALPORTWIDGET_H
