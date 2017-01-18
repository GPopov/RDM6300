#ifndef RDM6300_H
#define RDM6300_H
#include <initializer_list>
#include <array>
namespace RFID
{
    struct Payload
    {
        static const unsigned PAYLOAD_SIZE = 6;
        static const unsigned DATA_SIZE = 5;
        static const unsigned ID_SIZE = 3;
        static const unsigned MANUFACTURER_SIZE = 2;

        Payload() = default;
        Payload(const std::initializer_list<uint8_t>& list);

        bool operator==(const Payload& other) const;
        union
        {
            uint8_t m_Payload[PAYLOAD_SIZE];
            struct
            {
                uint8_t m_Data[DATA_SIZE];
                uint8_t m_Checksum;
            };
            struct
            {
                uint8_t m_Manufacturer[MANUFACTURER_SIZE];
                uint8_t m_ID[ID_SIZE];
                uint8_t m_Checksum;
            };
        };
        static_assert(PAYLOAD_SIZE - (ID_SIZE + MANUFACTURER_SIZE) == 1, "PAYLOAD_SIZE must be exactly 1 uint8_t bigger than ID_SIZE");
        static_assert(DATA_SIZE == ID_SIZE + MANUFACTURER_SIZE, "DATA_SIZE must be exactly ID_SIZE + MANUFACTURER_SIZE");
    };

    class RDM6300
    {
    public:
        bool decode(uint8_t input);
        RDM6300(Payload& p);
    private:

        enum Operation : uint8_t
        {
            Operation_BeginRead,
            Operation_ReadMSB,
            Operation_ReadLSB,
            Operation_ValidateChecksum,
            Operation_EndRead
        };
    private:
        uint8_t m_Cnt;
        Operation m_CurrentOperation;
        uint8_t m_Temp, m_Checksum;
        Payload& m_Payload;
    };

    static uint8_t ASCIIToByte(uint8_t input);
}
#endif

