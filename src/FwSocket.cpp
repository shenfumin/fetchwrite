#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <netinet/tcp.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include "FwSocket.h"


namespace FetchWrite
{

FwSocket::FwSocket(const char *ip, uint32_t port) : m_ip(ip), m_port(port)
{
}

FwSocket::~FwSocket()
{
    if (m_isConnected == true)
    {
        disconnect();
    }
}

bool FwSocket::connect()
{
    struct sockaddr_in peerAddr;
    bzero(&peerAddr, sizeof(peerAddr));
    peerAddr.sin_family      = AF_INET;
    peerAddr.sin_addr.s_addr = inet_addr(m_ip.c_str());
    peerAddr.sin_port        = htons(m_port);

    m_sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (::connect(m_sockFd, (struct sockaddr *)&peerAddr, sizeof(peerAddr)) < 0)
    {
        std::cerr << "Error on connecting: " << errno << "  " << strerror(errno) << std::endl;
    }
    else
    {
        m_isConnected = true;
        return true;
    }
    close(m_sockFd);
    m_sockFd      = -1;
    m_isConnected = false;
    return false;
}

bool FwSocket::hasError()
{
    if (m_sockFd == -1 || m_isConnected == false)
    {
        return true;
    }
    int32_t error  = 0;
    socklen_t len  = sizeof(error);
    int32_t retval = getsockopt(m_sockFd, SOL_SOCKET, SO_ERROR, &error, &len);
    if (retval != 0 || error != 0)
    {
        disconnect();
        return true;
    }
    return false;
}

bool FwSocket::setKeepAlive()
{
    int32_t yes = 1;
    if (setsockopt(m_sockFd, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int32_t)) == -1)
    {
        return false;
    }
    int32_t idle = 1;
    if (setsockopt(m_sockFd, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(int32_t)) == -1)
    {
        return false;
    }

    int32_t interval = 1;
    if (setsockopt(m_sockFd, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int32_t)) == -1)
    {
        return false;
    }
    int32_t maxpkt = 10;
    if (setsockopt(m_sockFd, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int32_t)) == -1)
    {
        return false;
    }
    return true;
}

void FwSocket::disconnect()
{
    if (m_isConnected)
    {
        close(m_sockFd);
        m_sockFd      = -1;
        m_isConnected = false;
    }
    std::cout << "FwSocket:disconnect" << std::endl;
}

bool FwSocket::write(std::vector<uint8_t> &sendData)
{
    uint32_t sentBytes = 0;
    if (sendData.empty())
    {
        std::cerr << "FwSocket: "
                  << "sendData is empty. \n"
                  << std::endl;
        return false;
    }
    if (!m_isConnected)
    {
        std::cerr << "FwSocket: "
                  << "socket is disconnected and write failed " << std::endl;
        return false;
    }
    do
    {
        int n = ::write(m_sockFd, &sendData[sentBytes], sendData.size() - sentBytes);
        if (n > 0)
        {
            sentBytes += n;
        }
        else
        {
            std::cerr << "FwSocket: Error sending errno: " << errno << "	 " << strerror(errno) << std::endl;
            break;
        }
    } while (sentBytes < sendData.size());

    if (sentBytes != sendData.size())
    {
        std::cerr << "FwSocket: write failed" << std::endl;
        disconnect();
        return false;
    }
    return true;
}

bool FwSocket::read(std::vector<uint8_t> &recvData)
{
    uint32_t recvCnt = 0;
    if (recvData.empty())
    {
        std::cerr << "FwSocket: "
                  << "recvData should assign space firstly" << std::endl;
        return false;
    }
    if (!m_isConnected)
    {
        std::cerr << "FwSocket: "
                  << "socket is disconneted when reading." << std::endl;
        return false;
    }
    struct timeval tv;
    tv.tv_sec  = TIMEOUT;
    tv.tv_usec = 0;
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(m_sockFd, &readfds);
    do
    {
        FD_ZERO(&readfds);
        FD_SET(m_sockFd, &readfds);
        int32_t ret = select(m_sockFd + 1, &readfds, NULL, NULL, &tv);
        if (ret <= -1)
        {
            std::cerr << "FwSocket:select " << strerror(errno) << std::endl;
            break;
        }
        else if (ret == 0)
        {
            std::cerr << "FwSocket:timeout when reading" << std::endl;
            break;
        }
        else if (ret > 0 && FD_ISSET(m_sockFd, &readfds))
        {
            int32_t n = ::read(m_sockFd, &recvData[recvCnt], recvData.size() - recvCnt);
            if (n > 0)
            {
                recvCnt += n;
            }
            else
            {
                std::cerr << "FwSocket:read " << strerror(errno) << std::endl;
                break;
            }
        }
        else
        {
            std::cerr << "FwSocket: ret=" << ret << std::endl;
            break;
        }
    } while (recvCnt < recvData.size());

    if (recvCnt != recvData.size())
    {
        std::cerr << "FwSocket: receive " << recvCnt << " is litte then" << recvData.size() << std::endl;
        disconnect();
        return false;
    }
    return true;
}

}
