#include <array>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include "Response.h"

namespace FetchWrite
{

Response::Response() : m_idx(0)
{
    m_data.resize(RES_HEADER_LEN);
}

Response::Response(int size) : m_idx(RES_HEADER_LEN)
{
    m_data.resize(RES_HEADER_LEN + size);
}

bool Response::hasErr()
{
    if (m_data[RES_OFFSET_ERR_FIELD] == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

std::vector<uint8_t> &Response::getData()
{
    return m_data;
}

template <>
Response &Response::operator>>(uint8_t &out)
{
    out = m_data.at(m_idx);
    ++m_idx;
    return *this;
}

template <>
Response &Response::operator>>(uint16_t &out)
{
    auto h = m_data.at(m_idx);
    ++m_idx;
    auto l = m_data.at(m_idx);
    ++m_idx;
    out = (h << 8 | l);
    return *this;
}

template <>
Response &Response::operator>>(std::vector<uint8_t> &out)
{
    for (auto iter = (m_data.cbegin() + m_idx); iter != m_data.cend(); ++iter)
    {
        out.emplace_back(*iter);
    }
    return *this;
}

std::ostream &operator<<(std::ostream &o, Response &resp)
{
    std::stringstream str;
    str.setf(std::ios_base::hex, std::ios::basefield);
    str.setf(std::ios_base::uppercase);
    str.fill('0');
    for (auto b : resp.getData())
    {
        str << " 0x" << std::setw(2) << (uint16_t)b;
    }
    o << str.str() << std::endl;
    return o;
}

}