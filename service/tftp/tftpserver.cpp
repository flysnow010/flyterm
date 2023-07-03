#include "tftpserver.h"
#include "serversocket.h"
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QHostAddress>
#include <iostream>

TFtpServer::TFtpServer(QObject *parent)
    : QObject(parent)
    , socket(new QUdpSocket(this))
    , fileManager_(new TFtpFileManager())
{
    connect(socket, &QUdpSocket::readyRead,
            this, &TFtpServer::readPendingDatagrams);
}

void TFtpServer::setFilePath(QString const& filePath)
{
    if(!filePath.endsWith("/"))
        filePath_ = filePath + "/";
}

void TFtpServer::start()
{
    socket->bind(TFTP_PORT);
}

void TFtpServer::stop()
{
    socket->close();
}

void TFtpServer::readPendingDatagrams()
{
    while (socket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = socket->receiveDatagram();
        QByteArray d = datagram.data();
        std::string transferId = QString("%1:%2").arg(datagram.senderAddress().toString())
                .arg(datagram.senderPort()).toStdString();
        TFtpServerFile::Ptr file = fileManager_->find(transferId);
        if(!file)
        {
            ServerSocket* udp = new ServerSocket(socket, datagram.senderAddress(), datagram.senderPort());
            file = TFtpServerFile::Ptr(new TFtpServerFile(udp, filePath_.toStdString(), transferId));
            fileManager_->add(file);
        }
        file->process((uint8_t *)d.data(), d.size());
        if(!file->is_finished())
        {
            if(file->type() == TFtpServerFile::Read)
                emit statusText(QString(tr("Downloding file: %1, progress: %4% blockNumber(%2/%3)"))
                                .arg(QString::fromStdString(file->filename()))
                                .arg(file->block_number())
                                .arg(file->block_numbers())
                                .arg(file->block_number() * 100 / file->block_numbers()));
            else
                emit statusText(QString(tr("Uploading file: %1, blockNumber(%2)"))
                                .arg(QString::fromStdString(file->filename()))
                                .arg(file->block_number()));
        }
        else
        {
            if(file->is_error())
                emit statusText(QString("%1:%2").arg((int)file->error()).arg(QString::fromStdString(file->error_msg())));
            else
                emit statusText(QString());
            fileManager_->remove(file->transfer_id());
        }
    }
}
