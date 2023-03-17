#include "udpsocket.h"
#include <QUdpSocket>


uint32_t UdpSocket::write(const char* data, uint32_t size)
{
    return socket_->writeDatagram(data, size, host_, port_);
}
