#include "sendfileprogressdialog.h"
#include "ui_sendfileprogressdialog.h"
#include <QApplication>
#include <QDebug>

SendFileProgressDialog::SendFileProgressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SendFileProgressDialog),
    filesize_(0),
    bytesOfSend_(0),
    isCancel_(false),
    isFinished_(false),
    time_(QTime::currentTime())
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
    ui->errorLabel->hide();
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
}

SendFileProgressDialog::~SendFileProgressDialog()
{
    delete ui;
}

void SendFileProgressDialog::setTitle(QString const& title)
{
    setWindowTitle(QString("%1: %2")
                   .arg(QApplication::applicationName(), title));
}

void SendFileProgressDialog::setFilename(QString const& filename)
{
    ui->fileNameEdit->setText(filename);
}

void SendFileProgressDialog::setProtocol(QString const& protocol)
{
    ui->protocolLabel->setText(protocol);
}

void SendFileProgressDialog::setFileSize(uint64_t filesize)
{
    filesize_ = filesize;
}

void SendFileProgressDialog::setProgressInfo(quint32 blockNumber, quint64 bytesOfSend)
{
    ui->packetLabel->setText(QString::number(blockNumber));
    ui->bytesLabel->setText(QString::number(bytesOfSend));

    if(filesize_ > 0)
    {
        int progress = bytesOfSend * 100 / filesize_;
        ui->progressBar->setValue(progress);
        QTime time = QTime::currentTime();
        int duration_ms = time_.msecsTo(time);
        if(duration_ms > 0)
        {
            double kBytes = (bytesOfSend - bytesOfSend_) / 1024.0;
            double rate = kBytes / duration_ms;
            int msecond = (filesize_ - bytesOfSend) / 1024.0 / rate;
            QString rateText = QString("%1 (%2KB/s)").arg(QTime(0,0,0).addSecs(msecond / 1000).toString("mm:ss"))
                                                     .arg(rate * 1000, 0, 'f', 2);
            ui->elapsedTimeLabel->setText(rateText);
        }
        bytesOfSend_ = bytesOfSend;
        time_ = time;
    }
}

void SendFileProgressDialog::finished()
{
    isFinished_ = true;
}

void SendFileProgressDialog::error(QString const& e)
{
    ui->errorLabel->setText(e);
    ui->errorLabel->show();
    qDebug() << e;
}

void SendFileProgressDialog::cancel()
{
    isCancel_ = true;
    reject();
}

