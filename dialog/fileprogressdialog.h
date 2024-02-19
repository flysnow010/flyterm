#ifndef FILEPROGRESSDIALOG_H
#define FILEPROGRESSDIALOG_H

#include <QDialog>
#include <QTime>

namespace Ui {
class FileProgressDialog;
}

class FileProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileProgressDialog(QWidget *parent = nullptr);
    ~FileProgressDialog();

    void setTitle(QString const& title);
    void setFilename(QString const& filename);
    void setProtocol(QString const& protocol);

    bool isCancel() const { return isCancel_; }
    bool isFinished() const { return isFinished_; }
    bool isError() const { return isError_; }
    bool isEnd() const { return (isCancel_ || isFinished_); }
public slots:
    void setFileSize(quint64 filesize);
    void setProgressInfo(quint32 blockNumber, quint64 bytesOfSend);
    void finished();
    void error(QString const& e);
private slots:
    void cancel();
private:
    Ui::FileProgressDialog *ui;
    quint64 filesize_;
    quint64 bytesOfSend_;
    volatile bool isCancel_;
    volatile bool isFinished_;
    volatile bool isError_;
    QTime time_;
};

#endif // FILEPROGRESSDIALOG_H
