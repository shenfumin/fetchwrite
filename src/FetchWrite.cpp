#include <iostream>
#include "FetchWrite.h"


namespace FetchWrite
{

FetchWrite::FetchWrite(const char *ip, int32_t writeport, int32_t fetchport, FW_MODE mode)
    : m_ip(ip),
      m_writePort(writeport),
      m_fetchPort(fetchport),
      m_mode(mode),
      m_fetchSocket(nullptr),
      m_writeSocket(nullptr)
{
    m_fetchSocket = std::make_shared<FwSocket>(m_ip.c_str(), m_fetchPort);
    if (writeport == fetchport)
    {
        m_writeSocket = m_fetchSocket;
    }
    else
    {
        m_writeSocket = std::make_shared<FwSocket>(m_ip.c_str(), m_writePort);
    }
}

boost::optional<FetchResponse> FetchWrite::fetch(Request &req)
{
    bool ret = false;
    if (m_fetchSocket->hasError())
    {
        if (false == m_fetchSocket->connect())
        {
            std::cout << "FW: "
                      << "fetch socket connect failed" << std::endl;
            return {};
        }
    }
    ret = m_fetchSocket->write(req.getData());
    if (ret == false)
    {
        std::cout << "FW: "
                  << "write failed when doing fetch" << std::endl;
        return {};
    }
    int len = req.getLength();
    if (m_mode == FetchWrite::MODE_S7)
    {
        len = len * 2;
    }
    FetchResponse res(len);
    ret = m_fetchSocket->read(res.getData());
    if (ret == false)
    {
        std::cout << "FW: "
                  << "read failed when doing fetch" << std::endl;
        return {};
    }
    return boost::make_optional(res);
}

boost::optional<WriteResponse> FetchWrite::write(Request &req)
{
    bool ret = false;
    if (m_writeSocket->hasError())
    {
        if (false == m_writeSocket->connect())
        {
            std::cout << "FW: "
                      << "write socket connect failed" << std::endl;
            return {};
        }
    }
    ret = m_writeSocket->write(req.getData());
    if (ret == false)
    {
        std::cout << "FW: "
                  << "write failed when doing write" << std::endl;
        return {};
    }
    WriteResponse res;
    ret = m_writeSocket->read(res.getData());
    if (ret == false)
    {
        std::cout << "FW: "
                  << "read failed when doing write" << std::endl;
        return {};
    }
    return boost::make_optional(res);
}

}
