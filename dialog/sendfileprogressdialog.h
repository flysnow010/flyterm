#ifndef SENDFILEPROGRESSDIALOG_H
#define SENDFILEPROGRESSDIALOG_H

#include <QDialog>
#include <QTime>

namespace Ui {
class SendFileProgressDialog;
}

class SendFileProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SendFileProgressDialog(QWidget *parent = nullptr);
    ~SendFileProgressDialog();

    void setTitle(QString const& title);
    void setFilename(QString const& filename);
    void setProtocol(QString const& protocol);

    bool isCancel() const { return isCancel_; }
    bool isFinished() const { return isFinished_; }
    bool isEnd() const { return (isCancel_ || isFinished_); }
public slots:
    void setFileSize(quint64 filesize);
    void setProgressInfo(quint32 blockNumber, quint64 bytesOfSend);
    void finished();
    void error(QString const& e);
private slots:
    void cancel();
private:
    Ui::SendFileProgressDialog *ui;
    quint64 filesize_;
    quint64 bytesOfSend_;
    volatile bool isCancel_;
    volatile bool isFinished_;
    QTime time_;
};

#endif // SENDFILEPROGRESSDIALOG_H
