#pragma once

#include <vector>

namespace FetchWrite
{

class Request
{
public:
    enum REQ_HEADER_OFFSET
    {
        REQ_OFFSET_SYS_ID            = 0,
        REQ_OFFSET_HEADER_LENGH      = 2,
        REQ_OFFSET_ID_OP_CODE        = 3,
        REQ_OFFSET_OP_CODE_LENGH     = 4,
        REQ_OFFSET_OP_CODE           = 5,
        REQ_OFFSET_ORG_FIELD         = 6,
        REQ_OFFSET_ORG_FIELD_LENGH   = 7,
        REQ_OFFSET_ORG_ID            = 8,
        REQ_OFFSET_DBNR              = 9,
        REQ_OFFSET_START_ADDR        = 10,
        REQ_OFFSET_LENGH             = 12,
        REQ_OFFSET_EMPTY_FIELD       = 14,
        REQ_OFFSET_EMPTY_FIELD_LENGH = 15
    };
    enum OP_CODE
    {
        OP_CODE_WRITE = 0x03,
        OP_CODE_FETCH = 0x05
    };

    Request(uint8_t opCode, uint8_t dbnr, uint16_t startAddress, uint16_t length);
    std::vector<uint8_t> &getData();
    template <class T>
    Request &operator<<(const T &);
    friend std::ostream &operator<<(std::ostream &o, Request &req);
    uint16_t getLength() const;

private:
    std::vector<uint8_t> m_data = {0x53, 0x35, 0x10, 0x01, 0x03, 0x05, 0x03, 0x08,
                                   0x01, 0x10, 0x01, 0x00, 0x00, 0x00, 0xff, 0x02};
};

class WriteRequest : public Request
{
public:
    WriteRequest(uint8_t dbnr, uint16_t startAddress, uint16_t length)
        : Request(Request::OP_CODE_WRITE, dbnr, startAddress, length){

          };
};

class FetchRequest : public Request
{
public:
    FetchRequest(uint8_t dbnr, uint16_t startAddress, uint16_t length)
        : Request(Request::OP_CODE_FETCH, dbnr, startAddress, length){

          };
};

}
