#include "connectdialog.h"
#include "ui_connectdialog.h"
#include "core/localshell.h"
#include "util/util.h"

#include <QtSerialPort/QSerialPortInfo>
#include <QFileDialog>

namespace  {
    int const SSH_TAB = 0;
    int const Telnet_Tab = 1;
    int const Serial_Tab = 2;
    int const Local_Tab = 3;
    int const WSL_Tab = 4;
}
ConnectDialog::ConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->tabWidget->tabBar()->setTabIcon(Local_Tab, Util::GetIcon("cmd.exe"));
    ui->tabWidget->tabBar()->setTabIcon(WSL_Tab, getOsIcon("windows"));

    fillPortsParameters();
    fillPortsInfo();
    fillLocalInfo();
    fillWSLInfo();

    connect(ui->toolButtonAddFile, SIGNAL(clicked()),
            this, SLOT(selectPrivateKeyFileName()));
    connect(ui->toolButtonShellSelectPath, SIGNAL(clicked()),
            this, SLOT(selectSelectShellPath()));
    connect(ui->toolButtonWSLStartupDirectory, SIGNAL(clicked()),
            this, SLOT(selectSelectWSLPath()));
}

ConnectDialog::~ConnectDialog()
{
    delete ui;
}

ConnectType ConnectDialog::type() const
{
    int currentIndex = ui->tabWidget->currentIndex();
    if(currentIndex == SSH_TAB)
        return SSH;
    if(currentIndex == Telnet_Tab)
        return Telnet;
    else if(currentIndex == Serial_Tab)
        return Serial;
    else if (currentIndex == Local_Tab)
        return Local;
    else if (currentIndex == WSL_Tab)
        return WSL;
    else
        return None;
}

void ConnectDialog::setType(ConnectType t)
{
    if(t == SSH)
        ui->tabWidget->setCurrentIndex(SSH_TAB);
    else if(t == Telnet)
        ui->tabWidget->setCurrentIndex(Telnet_Tab);
    else if(t == Serial)
        ui->tabWidget->setCurrentIndex(Serial_Tab);
    else if(t == Local)
        ui->tabWidget->setCurrentIndex(Local_Tab);
    else if(t == WSL)
        ui->tabWidget->setCurrentIndex(WSL_Tab);
}

SSHSettings ConnectDialog::sshSettings() const
{
    SSHSettings settings;
    settings.hostName = ui->sshHostname->text();
    settings.userName = ui->sshUsername->text();
    settings.port = ui->sshPort->value();
    if(ui->usePrivateKey->isChecked())
    {
        settings.privateKeyFileName = ui->privateKeyFileName->text();
        if(!settings.privateKeyFileName.isEmpty())
            settings.usePrivateKey = true;
    }
    return settings;
}

void ConnectDialog::setSshSettings(SSHSettings const& settings)
{
    ui->sshHostname->setText(settings.hostName);
    ui->sshUsername->setText(settings.userName);
    ui->sshPort->setValue(settings.port);
    ui->usePrivateKey->setChecked(settings.usePrivateKey);
    ui->privateKeyFileName->setText(settings.privateKeyFileName);
}

TelnetSettings ConnectDialog::telnetSettings() const
{
    TelnetSettings settings;
    settings.hostName = ui->telnetHostname->text();
    settings.userName = ui->telnetUsername->text();
    settings.port = ui->telnetPort->value();
    return settings;
}

void ConnectDialog::setTelnetSettings(TelnetSettings const& settings)
{
    ui->telnetHostname->setText(settings.hostName);
    ui->telnetUsername->setText(settings.userName);
    ui->telnetPort->setValue(settings.port);
}

SerialSettings ConnectDialog::serialSettings()  const
{
    SerialSettings settings;
    settings.port = ui->comboBoxPort->currentData().toString();
    settings.baudRate = ui->comboBoxBaudRate->currentData().toInt();
    settings.dataBits = static_cast<QSerialPort::DataBits>(ui->comboBoxDataBits->currentData().toInt());
    settings.parity = static_cast<QSerialPort::Parity>(ui->comboBoxParity->currentData().toInt());
    settings.stopBits = static_cast<QSerialPort::StopBits>(ui->comboBoxStopBits->currentData().toInt());
    settings.flowControl = static_cast<QSerialPort::FlowControl>(ui->comboBoxFlowControl->currentData().toInt());
    settings.localEchoEnabled = ui->checkBoxLocalEcho->isChecked();
    return settings;
}

void ConnectDialog::setSerialSettings(SerialSettings const& settings)
{
    ui->comboBoxPort->setCurrentIndex(ui->comboBoxPort->findData(settings.port));
    ui->comboBoxBaudRate->setCurrentIndex(ui->comboBoxBaudRate->findData(settings.baudRate));
    ui->comboBoxDataBits->setCurrentIndex(ui->comboBoxDataBits->findData(settings.dataBits));
    ui->comboBoxParity->setCurrentIndex(ui->comboBoxParity->findData(settings.parity));
    ui->comboBoxStopBits->setCurrentIndex(ui->comboBoxStopBits->findData(settings.stopBits));
    ui->comboBoxFlowControl->setCurrentIndex(ui->comboBoxFlowControl->findData(settings.flowControl));
    ui->checkBoxLocalEcho->setChecked(settings.localEchoEnabled);
}

LocalShellSettings ConnectDialog::localShellSettings() const
{
    LocalShellSettings settings;
    settings.shellText = ui->comboBoxSheelType->currentText();
    settings.shellType = ui->comboBoxSheelType->currentData().toString();
    settings.executeCommand = ui->comboBoxShellExecuteCommand->currentText();
    settings.startupPath = ui->lineEditShellStartupPath->text();
    return settings;
}

void ConnectDialog::setLocalShellSettings(LocalShellSettings const& settings)
{
    QStringList shellTypes = QStringList() << "cmd" << "powershell";
    ui->comboBoxSheelType->setCurrentIndex(shellTypes.indexOf(settings.shellType));
    ui->comboBoxShellExecuteCommand->setCurrentText(settings.executeCommand);
    ui->lineEditShellStartupPath->setText(settings.startupPath);
}

WSLSettings ConnectDialog::wslSettings() const
{
    WSLSettings settings;
    settings.distributionText = ui->comboBoxWSLDistribution->currentText();
    settings.distribution = ui->comboBoxWSLDistribution->currentData().toString();
    settings.startupPath = ui->lineEditWSLStartupPath->text();
    settings.specifyUsername = ui->lineEditWSLUsername->text();
    settings.useSpecifyUsername = ui->checkBoxUseSpecifyUsername->isChecked();
    return settings;
}

void ConnectDialog::setWslSettings(WSLSettings const& settings)
{
    ui->comboBoxWSLDistribution->setCurrentIndex(distributionItmes.indexOf(settings.distribution));
    ui->lineEditShellStartupPath->setText(settings.startupPath);
    ui->lineEditWSLUsername->setText(settings.specifyUsername);
    ui->checkBoxUseSpecifyUsername->setChecked(settings.useSpecifyUsername);
}

void ConnectDialog::selectPrivateKeyFileName()
{
    static QString filePath;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Private Key File"),
                                                    filePath,
                                                    tr("Private Key File (*.*)"));
    if(!fileName.isEmpty())
    {
        filePath = QFileInfo(fileName).filePath();
        ui->privateKeyFileName->setText(fileName);
    }
}

void ConnectDialog::selectSelectShellPath()
{
    QString startypPath = QFileDialog::getExistingDirectory(this, tr("Select shell startup path"),
                                      ui->lineEditShellStartupPath->text());
    if(!startypPath.isEmpty())
        ui->lineEditShellStartupPath->setText(startypPath);
}

void ConnectDialog::selectSelectWSLPath()
{
    QString startypPath = QFileDialog::getExistingDirectory(this, tr("Select WSL startup path"),
                                      ui->lineEditWSLStartupPath->text());
    if(!startypPath.isEmpty())
        ui->lineEditWSLStartupPath->setText(startypPath);
}

void ConnectDialog::fillPortsParameters()
{
    ui->comboBoxBaudRate->addItem(QStringLiteral("1200"), QSerialPort::Baud1200);
    ui->comboBoxBaudRate->addItem(QStringLiteral("2400"), QSerialPort::Baud2400);
    ui->comboBoxBaudRate->addItem(QStringLiteral("4800"), QSerialPort::Baud4800);
    ui->comboBoxBaudRate->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->comboBoxBaudRate->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->comboBoxBaudRate->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->comboBoxBaudRate->addItem(QStringLiteral("57600"), QSerialPort::Baud57600);
    ui->comboBoxBaudRate->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->comboBoxBaudRate->setCurrentIndex(7);

    ui->comboBoxDataBits->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->comboBoxDataBits->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->comboBoxDataBits->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->comboBoxDataBits->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->comboBoxDataBits->setCurrentIndex(3);

    ui->comboBoxParity->addItem(tr("None"), QSerialPort::NoParity);
    ui->comboBoxParity->addItem(tr("Even"), QSerialPort::EvenParity);
    ui->comboBoxParity->addItem(tr("Odd"), QSerialPort::OddParity);
    ui->comboBoxParity->addItem(tr("Mark"), QSerialPort::MarkParity);
    ui->comboBoxParity->addItem(tr("Space"), QSerialPort::SpaceParity);

    ui->comboBoxStopBits->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->comboBoxStopBits->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
    ui->comboBoxStopBits->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    ui->comboBoxFlowControl->addItem(tr("None"), QSerialPort::NoFlowControl);
    ui->comboBoxFlowControl->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    ui->comboBoxFlowControl->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
}

void ConnectDialog::fillPortsInfo()
{
    ui->comboBoxPort->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        ui->comboBoxPort->addItem(QString("%1: %2")
                                  .arg(info.portName(), info.description()),
                                  info.portName());
    }
}

void ConnectDialog::fillLocalInfo()
{
    ui->comboBoxSheelType->addItem(Util::GetIcon("cmd.exe"), tr("Command Prompt"), "cmd");
    ui->comboBoxSheelType->addItem(Util::GetIcon("powershell.exe"), tr("Windows PowerShell"), "powershell");
}

void ConnectDialog::fillWSLInfo()
{
    ui->comboBoxWSLDistribution->addItem(getOsIcon("windows"), tr("Default"), "");
    LocalShell localShell;
    connect(&localShell, &LocalShell::onData, this, [=](QByteArray const& data){
        QStringList items = Util::fromUnicode(data).split("\r\n");
        foreach(auto item, items)
        {
            if(!item.isEmpty())
            {
                ui->comboBoxWSLDistribution->addItem(getOsIcon(item.toLower()), item, item);
                distributionItmes << item;
            }
        }

    });

    localShell.start("wsl", QStringList() << "-l" << "-q");
    localShell.wait();
}

QIcon ConnectDialog::getOsIcon(QString const& os)
{
    QStringList icons = QStringList()
            << "windows" << "ubuntu" << "debian" << "kali" << "oracle" << "suse";
    foreach(auto icon, icons)
    {
        if(os.contains(icon))
            return QIcon(QString(":image/Os/%1.png").arg(icon));
    }
    return QIcon(":image/Os/linux.png");
}
