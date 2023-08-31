#ifndef SERIALPORTWIDGET_H
#define SERIALPORTWIDGET_H

#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include "core/logfile.h"
#include "core/serialsettings.h"
#include "color/consolecolor.h"
#include "color/consolepalette.h"

class QMenu;
class Console;
class CommandThread;
class SerialPortWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SerialPortWidget(bool isLog, QWidget *parent = nullptr);
    ~SerialPortWidget();

    bool runShell(QString const& name, int bandrate);
    bool runShell(SerialSettings const& settings);
    void sendCommand(QString const& command);

    QSize sizeHint() const override;
    QString errorString();
    void setErrorText(QString const& text);

    void disconnect();
    bool isDisplay() const;
    void display();
    void undisplay();
    void setCodecName(QString const& name);
    void setFontName(QString const& name);
    void setConsoleColor(ConsoleColor const& color);
    void setConsolePalette(ConsolePalette::Ptr palette);
    void setFontSize(int fontSize);
    void print();
    void updateHightLighter(QString const& hightLighter);
    void setHighLighter(QString const& hightLighter);

public slots:
    void recvFileByKermit();
    void sendFileByKermit();
    void recvFileByXModem();
    void sendFileByXModem();
    void recvFileByYModem();
    void sendFileByYModem();
    void save();
    void copy();
    void copyAll();
    void paste();
    void increaseFontSize();
    void decreaseFontSize();
    void clearScrollback();
signals:
    void onClose(QWidget *widget);
    void onCommand(QString const& command);
    void fontSizeChanged(int fonstSize);
    void getHighlighter();
    void highLighterChanged(QString const& highLight);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
private slots:
    void readData();
    void pullData();
    void writeData(QByteArray const&data);
    void customContextMenu(const QPoint &);
    void setHighLighter();
    void onGotCursorPos(int row, int col);
    void execCommand(QString const& command);
    void execTestCommand(QString const& command);
    void execExpandCommand(QString const& command);
private:
    void createHighLightMenu(QMenu* menu);
    void sendCommands(QStringList const& commands);
    void sendFileByKermit(QString const& fileName);
    void recvFileByKermit(QString const& fileName);
    void sendFileByXYModem(QString const& fileName, bool isYModem);
    void recvFileByXYModem(QString const& fileName, bool isYModem);

    QString getTestCommand();
private:
    Console* console;
    CommandThread* commandThread_;
    QSerialPort* serial;
    QTimer* dataTimer;
    QByteArrayList datas;
    LogFile::SharedPtr beforeLogfile_;
    LogFile::SharedPtr afterLogfile_;
    QString highLight_;
    QString filePath;
    QStringList testCommands_;
    QByteArray testParam_;
    bool isTest_ = false;
    QByteArray testData_;
};

#endif // SERIALPORTWIDGET_H
