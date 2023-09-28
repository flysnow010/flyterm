#include "wslwidget.h"
#include "console/wslconsole.h"
#include "core/localshell.h"
#include "core/wslsettings.h"
#include "util/util.h"

WSLWidget::WSLWidget(bool isLog, QWidget *parent)
   : Child(parent)
   , shell(new LocalShell(this))
{
    createConsoleAndConnect();
    connect(shell, SIGNAL(onData(QByteArray)), this, SLOT(onData(QByteArray)));
    connect(this, &Child::onClose, this, [=](QWidget *){
        shell->stop();
    });

    if(isLog)
    {
        beforeLogfile_ = LogFile::SharedPtr(new LogFile());
        beforeLogfile_->open(QString("%1/wsl_%2_%3.txt")
                       .arg(Util::logoPath())
                       .arg(uint64_t(this), 8, 16)
                       .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss")));
    }
}

bool WSLWidget::runShell(WSLSettings const& settings)
{
    QStringList params;
    params << "--headless";
    params << "--width" << "160";
    params << "--height" << "55";
    params << "wsl.exe";
    if(!settings.distribution.isEmpty())
        params << "-d" << settings.distribution;
    if(!settings.startupPath.isEmpty())
        params << "--cd" << settings.startupPath;
    shell->start("conhost", params);
    isConnected_ = true;
    display();
    return true;
}

void WSLWidget::reconnect(WSLSettings const& settings)
{
    clearScrollback();
    runShell(settings);
}

void WSLWidget::disconnect()
{
    shell->stop();
    isConnected_ = false;
}

Console* WSLWidget::createConsole()
{
    return new WSLConsole(this);
}

void WSLWidget::writeToShell(QByteArray const& data)
{
    shell->write(data);
}

void WSLWidget::onPullData()
{
    if(datas.isEmpty())
        return;

    QByteArray data;
    while(datas.size() > 0 && data.size() < 512)
        data.push_back(datas.takeFirst());
    onDisplay(data);
    if(beforeLogfile_)
        beforeLogfile_->write(data);
}

void WSLWidget::onData(QByteArray const& data)
{
    datas << data;
}
