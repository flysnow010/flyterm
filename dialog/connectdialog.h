#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>
#include "core/connecttype.h"
#include "core/sshsettings.h"
#include "core/telnetsettings.h"
#include "core/serialsettings.h"
#include "core/localshellsettings.h"
#include "core/wslsettings.h"

namespace Ui {
class ConnectDialog;
}

class ConnectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectDialog(QWidget *parent = nullptr);
    ~ConnectDialog();

    ConnectType type() const;
    void setType(ConnectType t);

    SSHSettings sshSettings() const;
    void setSshSettings(SSHSettings const& settings);

    TelnetSettings telnetSettings() const;
    void setTelnetSettings(TelnetSettings const& settings);

    SerialSettings serialSettings()  const;
    void setSerialSettings(SerialSettings const& settings);

    LocalShellSettings localShellSettings() const;
    void setLocalShellSettings(LocalShellSettings const& settings);

    WSLSettings wslSettings() const;
    void setWslSettings(WSLSettings const& settings);
private slots:
    void selectPrivateKeyFileName();
    void selectSelectShellPath();
    void selectSelectWSLPath();
private:
    void fillPortsParameters();
    void fillPortsInfo();
    void fillLocalInfo();
    void fillWSLInfo();
    QIcon getOsIcon(QString const& os);
private:
    Ui::ConnectDialog *ui;
    QStringList distributionItmes;
};

#endif // CONNECTDIALOG_H
