#include "telnetwidget.h"
#include "qttelnet.h"
#include "console/telnetconsole.h"
#include "core/telnetsettings.h"
#include "util/util.h"

TelnetWidget::TelnetWidget(bool isLog, QWidget *parent)
    : Child(parent)
    , telnet(new QtTelnet(this))
{
    createConsoleAndConnect();
    connect(telnet, SIGNAL(message(QString)), this, SLOT(onData(QString)));
    connect(this, &Child::onClose, this, [=](QWidget *){
        telnet->close();
    });

    if(isLog)
    {
        beforeLogfile_ = LogFile::SharedPtr(new LogFile());
        beforeLogfile_->open(QString("%1/telnet_%2_%3.txt")
                       .arg(Util::logPath())
                       .arg(uint64_t(this), 8, 16)
                       .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss")));
    }
}

void TelnetWidget::resizeEvent(QResizeEvent *event)
{
    Child::resizeEvent(event);
    QFontMetrics fm(font());
    int lh = fm.lineSpacing();
    int cw = fm.width(QChar('X'));

    telnet->setWindowSize(event->size().width()/ cw, event->size().height() / lh);
}

Console* TelnetWidget::createConsole()
{
    return new TelnetConsole(this);
}

void TelnetWidget::writeToShell(QByteArray const& data)
{
    telnet->sendData(QString::fromUtf8(data));
}

void TelnetWidget::onPullData()
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

QString TelnetWidget::errorString()
{
    return QString();
}

void TelnetWidget::setErrorText(QString const& text)
{
    setConsoleText(text);
}

void TelnetWidget::reconnect(TelnetSettings const& settings)
{
    clearScrollback();
    runShell(settings);
}

void TelnetWidget::disconnect()
{
    telnet->close();
    isConnected_ = false;
}

bool TelnetWidget::runShell(TelnetSettings const& settings)
{
    telnet->login(settings.userName, QString());
    telnet->connectToHost(settings.hostName, settings.port);
    isConnected_ = true;
    display();
    return true;
}

void TelnetWidget::onData(QString const& data)
{
    texts << data;
}

