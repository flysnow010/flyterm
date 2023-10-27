#include "localshellwidget.h"
#include "console/localshellconsole.h"
#include "core/localshell.h"
#include "core/localshellsettings.h"
#include "util/util.h"

LocalShellWidget::LocalShellWidget(bool isLog, QWidget *parent)
    : Child(parent)
    , shell(new LocalShell(this))
{
    createConsoleAndConnect();
    connect(shell, SIGNAL(onData(QByteArray)), this, SLOT(onData(QByteArray)));
    connect(console(), SIGNAL(onCommand(QString)), this, SLOT(setCommand(QString)));
    connect(this, &Child::onClose, this, [=](QWidget *){
        shell->stop();
    });

    if(isLog)
    {
        beforeLogfile_ = LogFile::SharedPtr(new LogFile());
        beforeLogfile_->open(QString("%1/localshell_%2_%3.txt")
                       .arg(Util::logPath())
                       .arg(uint64_t(this), 8, 16)
                       .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss")));
    }
}

bool LocalShellWidget::runShell(LocalShellSettings const& settings)
{
    if(settings.shellType == "cmd")
        return runCmdShell(settings);
    else if(settings.shellType == "powershell")
        return runPowerShell(settings);
    return false;
}

bool LocalShellWidget::runCmdShell(LocalShellSettings const& settings)
{
    console()->setLocalEchoEnabled(true);
    console()->setNeedNewLine(true);
    QStringList params;
    if(!settings.startupPath.isEmpty())
        shell->setWorkingDirectory(settings.getCurrentPath());
    if(!settings.executeCommand.isEmpty())
        params << "/k" << settings.executeCommand.split(" ");
    shell->start(settings.shellType, params);
    isConnected_ = true;
    display();
    return true;
}

bool LocalShellWidget::runPowerShell(LocalShellSettings const& settings)
{
    QStringList params;
    params << "-nologo";
    if(!settings.startupPath.isEmpty())
        shell->setWorkingDirectory(settings.getCurrentPath());

    if(!settings.executeCommand.isEmpty())
        params << "-NoExit" << "-Command" << settings.executeCommand.split(" ");

    shell->start(settings.shellType, params);
    isConnected_ = true;
    display();
    return true;
}

void LocalShellWidget::reconnect(LocalShellSettings const& settings)
{
    clearScrollback();
    runShell(settings);
}

void LocalShellWidget::disconnect()
{
    shell->stop();
    isConnected_ = false;
}

Console* LocalShellWidget::createConsole()
{
    return new LocalShellConsole(this);
}

void LocalShellWidget::writeToShell(QByteArray const& data)
{
    shell->write(data);
}

void LocalShellWidget::onData(QByteArray const& data)
{
    QString text = QString::fromLocal8Bit(data);
    if(!text.isEmpty() && text.startsWith(command_))
    {
        text = text.mid(command_.size());
        command_ = "";
    }
    texts << text;
}

void LocalShellWidget::writeData(QByteArray const&data)
{
    shell->write(data);
}

void LocalShellWidget::onPullData()
{
    if(texts.isEmpty())
        return;

    QString data;
    while(texts.size() > 0 && data.size() < 512)
        data.push_back(texts.takeFirst());
    onDisplay(data.toUtf8());
    if(beforeLogfile_)
        beforeLogfile_->write(data);
}

void LocalShellWidget::setCommand(QString const& command)
{
    command_ = command;
}
