#include "stdafx.h"
#include "RDM6300.h"
namespace RFID
{
Payload::Payload(const std::initializer_list<uint8_t>& list)
{
    unsigned i = 0;
    for (auto d : list)
    {
        m_Payload[i] = d;
        ++i;
    }
}

bool Payload::operator==(const Payload& other) const
{
    if (m_Checksum != other.m_Checksum)
    {
        return false;
    }

    for (size_t i = 0; i < MANUFACTURER_SIZE; ++i)
    {
        if (m_Manufacturer[i] != other.m_Manufacturer[i])
            return false;
    }

    for (size_t i = 0; i < ID_SIZE; ++i)
    {
        if (m_ID[i] != other.m_ID[i])
            return false;
    }
    return true;
}

RDM6300::RDM6300(Payload& p)    
    : m_Cnt(0)
    , m_CurrentOperation(Operation_BeginRead)
    , m_Checksum(0)
    , m_Temp(0)
    , m_Payload(p){

}

bool RDM6300::decode(uint8_t input) {

    if (input == 0x02)
    {
        m_Cnt = 0;
        m_Checksum = 0;        
        m_Temp = 0;
        m_CurrentOperation = Operation_BeginRead;
    }

    switch (m_CurrentOperation)
    {
        case Operation_BeginRead:
            if (input == 0x02) {
                m_CurrentOperation = Operation_ReadMSB;
            }
            break;                    
        case Operation_ReadMSB:
            m_Temp = ASCIIToByte(input);
            m_CurrentOperation = Operation_ReadLSB;
            break;
        case Operation_ReadLSB:
            input = ASCIIToByte(input);
            m_Temp = (m_Temp << 4) | input;
            m_Payload.m_Payload[m_Cnt++] = m_Temp;
            if (m_Cnt > Payload::DATA_SIZE) {
                m_CurrentOperation = Operation_EndRead;
            }
            else {
                m_CurrentOperation = Operation_ReadMSB;
                m_Checksum ^= m_Temp;
            }
            break;
        case Operation_EndRead:
            if (input == 0x03) {
                m_CurrentOperation = Operation_ValidateChecksum;
            }
            else {
                m_CurrentOperation = Operation_BeginRead;
            }
        case Operation_ValidateChecksum:
            break;
    }

    if (m_CurrentOperation == Operation_ValidateChecksum)
    {
        m_CurrentOperation = Operation_BeginRead;
        m_Cnt = 0;
        if (m_Checksum == m_Payload.m_Checksum) {
            return true;
        }
    }
        
    return false;
}

uint8_t ASCIIToByte(uint8_t input)
{
    // 0x30 - 0x39 : ASCII '0' - '9'
    // 0x41 - 0x46 : ASCII 'A' - 'F'

    uint8_t val = input;
    val -= 0x30; //0x30 is '0' in the ASCII table
    if (val > 9)
        val -= 7; //There are 7 symbols between 9 and A in the ASCII table- namely : ; < = > ? @
    return val;
}
}