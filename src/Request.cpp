#include <assert.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include "Request.h"

namespace FetchWrite
{

Request::Request(uint8_t opCode, uint8_t dbnr, uint16_t startAddress, uint16_t length)
{
    assert(dbnr <= 255);
    assert(startAddress <= 2047);
    assert(length <= 4096);

    m_data[REQ_OFFSET_OP_CODE]        = opCode;
    m_data[REQ_OFFSET_DBNR]           = dbnr;
    m_data[REQ_OFFSET_START_ADDR]     = (startAddress >> 8) & 0xff;
    m_data[REQ_OFFSET_START_ADDR + 1] = startAddress & 0xff;
    m_data[REQ_OFFSET_LENGH]          = (length >> 8) & 0xff;
    m_data[REQ_OFFSET_LENGH + 1]      = length & 0xff;
}

uint16_t Request::getLength() const
{
    uint8_t length = m_data[REQ_OFFSET_LENGH];
    return ((length << 8) | m_data[REQ_OFFSET_LENGH + 1]);
}

std::vector<uint8_t> &Request::getData()
{
    return m_data;
}

template <>
Request &Request::operator<<(const uint16_t &data)
{
    m_data.emplace_back((data >> 8 & 0xff));
    m_data.emplace_back((data & 0xff));
    return *this;
}

template <>
Request &Request::operator<<(const std::vector<uint8_t> &data)
{
    m_data.insert(m_data.end(), data.begin(), data.end());
    return *this;
}

template <>
Request &Request::operator<<(const std::vector<uint16_t> &data)
{
    for (auto d : data)
    {
        m_data.emplace_back((d >> 8 & 0xff));
        m_data.emplace_back((d & 0xff));
    }
    return *this;
}

std::ostream &operator<<(std::ostream &o, Request &req)
{
    std::stringstream str;
    str.setf(std::ios_base::hex, std::ios::basefield);
    str.setf(std::ios_base::uppercase);
    str.fill('0');
    for (auto b : req.getData())
    {
        str << " 0x" << std::setw(2) << (uint16_t)b;
    }
    o << str.str() << std::endl;
    return o;
}

}