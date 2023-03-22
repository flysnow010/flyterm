#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H
#include "baseudp.h"
#include <QHostAddress>

class QUdpSocket;
class ServerSocket : public BaseUdp
{
public:
    ServerSocket(QUdpSocket* socket, QHostAddress const& host,
              uint16_t port)
        : socket_(socket)
        , host_(host)
        , port_(port)
    {}

    uint32_t write(const char* data, size_t size) override;
private:
    QUdpSocket* socket_;
    QHostAddress host_;
    uint16_t port_;
};

#endif // SERVERSOCKET_H
