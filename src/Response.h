#pragma once

#include <vector>

namespace FetchWrite
{

class Response
{
public:
    constexpr static short RES_HEADER_LEN = 16;
    enum RES_HEADER_OFFSET
    {
        RES_OFFSET_SYS_ID            = 0,
        RES_OFFSET_HEADER_LENGH      = 2,
        RES_OFFSET_ID_OP_CODE        = 3,
        RES_OFFSET_OP_CODE_LENGH     = 4,
        RES_OFFSET_OP_CODE           = 5,
        RES_OFFSET_ACK_FIELD         = 6,
        RES_OFFSET_ACK_FIELD_LENGH   = 7,
        RES_OFFSET_ERR_FIELD         = 8,
        RES_OFFSET_EMPTY_FIELD       = 9,
        RES_OFFSET_EMPTY_FIELD_LENGH = 10
    };
    enum OP_CODE
    {
        OP_CODE_WRITE = 0x03,
        OP_CODE_FETCH = 0x05
    };

    Response();
    Response(int size);
    void resize(uint16_t size);
    std::vector<uint8_t> &getData();
    bool hasErr();
    template <class T>
    Response &operator>>(T &);
    friend std::ostream &operator<<(std::ostream &o, Response &resp);

private:
    std::vector<uint8_t> m_data;
    int m_idx;
};

class WriteResponse : public Response
{
public:
    WriteResponse()
        : Response(){

          };
};

class FetchResponse : public Response
{
public:
    FetchResponse() = delete;
    FetchResponse(int size) : Response(size)
    {
    }
};

}