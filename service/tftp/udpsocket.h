#ifndef UDPSOCKET_H
#define UDPSOCKET_H
#include "baseudp.h"
#include <QHostAddress>

class QUdpSocket;
class UdpSocket : public BaseUdp
{
public:
    UdpSocket(QUdpSocket* socket, QHostAddress const& host,
              uint16_t port)
        : socket_(socket)
        , host_(host)
        , port_(port)
    {}

    uint32_t write(const char* data, uint32_t size) override;
private:
    QUdpSocket* socket_;
    QHostAddress host_;
    uint16_t port_;
};

#endif // UDPSOCKET_H
