#pragma once

#include <string>
#include <vector>

namespace FetchWrite
{

class FwSocket
{
public:
    FwSocket(const char *ip, uint32_t port);
    ~FwSocket();
    FwSocket() = delete;
    bool write(std::vector<uint8_t> &sendData);
    bool read(std::vector<uint8_t> &recvData);
    bool connect();
    void disconnect();
    bool hasError();

private:
    bool setKeepAlive();
    constexpr static uint32_t TIMEOUT = 1; // second
    int m_sockFd                      = -1;
    bool m_isConnected                = false;
    std::string m_ip;
    uint32_t m_port;
};

}