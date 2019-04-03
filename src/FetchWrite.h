#pragma once

#include <boost/optional.hpp>
#include <memory>
#include "FwSocket.h"
#include "Request.h"
#include "Response.h"

namespace FetchWrite
{

class FetchWrite
{
public:
    enum FW_MODE
    {
        MODE_S5 = 1, // byte mode
        MODE_S7 = 2  // word mode
    };
    FetchWrite(const char *ip, int32_t writeport, int32_t fetchport, FW_MODE mode = MODE_S7);
    boost::optional<FetchResponse> fetch(Request &req);
    boost::optional<WriteResponse> write(Request &req);
    FetchWrite()                   = delete;
    FetchWrite(FetchWrite const &) = delete;
    void operator=(FetchWrite const &) = delete;

private:
    std::string m_ip;
    int32_t m_writePort;
    int32_t m_fetchPort;
    FW_MODE m_mode;
    std::shared_ptr<FwSocket> m_fetchSocket;
    std::shared_ptr<FwSocket> m_writeSocket;
};

}
