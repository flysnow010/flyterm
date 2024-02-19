#include "fileprogressdialog.h"
#include "ui_fileprogressdialog.h"

#include <QApplication>
#include <QDebug>
#include <QTimer>

int const WAIT_TIME_FOR_CLOSE = 5000;

FileProgressDialog::FileProgressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileProgressDialog),
    filesize_(0),
    bytesOfSend_(0),
    isCancel_(false),
    isFinished_(false),
    isError_(false),
    time_(QTime::currentTime())
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
    ui->errorLabel->hide();
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
}

FileProgressDialog::~FileProgressDialog()
{
    delete ui;
}

void FileProgressDialog::setTitle(QString const& title)
{
    setWindowTitle(QString("%1: %2")
                   .arg(QApplication::applicationName(), title));
}

void FileProgressDialog::setFilename(QString const& filename)
{
    ui->fileNameEdit->setText(filename);
}

void FileProgressDialog::setProtocol(QString const& protocol)
{
    ui->protocolLabel->setText(protocol);
}

void FileProgressDialog::setFileSize(uint64_t filesize)
{
    filesize_ = filesize;
}

void FileProgressDialog::setProgressInfo(quint32 blockNumber, quint64 bytesOfSend)
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

void FileProgressDialog::finished()
{
    isFinished_ = true;
}

void FileProgressDialog::error(QString const& e)
{
    ui->errorLabel->setText(QString(tr("%1(Auto closed after %2 seconds)"))
                            .arg(e).arg(WAIT_TIME_FOR_CLOSE / 1000));
    ui->errorLabel->show();
    isError_ = true;
    QTimer::singleShot(WAIT_TIME_FOR_CLOSE, this, SLOT(finished()));
}

void FileProgressDialog::cancel()
{
    isCancel_ = true;
    reject();
}
