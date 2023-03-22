#include "serversocket.h"
#include <QUdpSocket>


uint32_t ServerSocket::write(const char* data, size_t size)
{
    return socket_->writeDatagram(data, size, host_, port_);
}
