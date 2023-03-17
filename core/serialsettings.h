#ifndef SERIALSETTINGS_H
#define SERIALSETTINGS_H
#include <QString>
#include <QtSerialPort/QSerialPort>

struct SerialSettings {
    QString port;
    qint32 baudRate = 115200;
    QSerialPort::DataBits dataBits = QSerialPort::Data8;
    QSerialPort::Parity parity = QSerialPort::NoParity;
    QSerialPort::StopBits stopBits = QSerialPort::OneStop;
    QSerialPort::FlowControl flowControl = QSerialPort::NoFlowControl;
    bool localEchoEnabled = true;
};
#endif // SERIALSETTINGS_H
