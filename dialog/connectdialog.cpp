#include "connectdialog.h"
#include "ui_connectdialog.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QFileDialog>

namespace  {
    int const SSH_TAB = 0;
    int const Telnet_Tab = 1;
    int const Serial_Tab = 2;
}
ConnectDialog::ConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    fillPortsParameters();
    fillPortsInfo();
    connect(ui->toolButtonAddFile, SIGNAL(clicked()),
            this, SLOT(selectPrivateKeyFileName()));
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
    else
        return None;
}

void ConnectDialog::setType(ConnectType t)
{
    if(t == SSH)
        ui->tabWidget->setCurrentIndex(0);
    else if(t == Telnet)
        ui->tabWidget->setCurrentIndex(1);
    else if(t == Serial)
        ui->tabWidget->setCurrentIndex(2);
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
                                  .arg(info.portName())
                                  .arg(info.description()),
                                  info.portName());
    }
}
