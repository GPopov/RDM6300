/*
  RFID RDM6300 data decoder library
  (c) Stephane Driussi 20150623
  Not for commercial use
*/

#ifndef RDM6300_H
#define RDM6300_H

class RDM6300 {
public:
    bool decode(uint8_t input);
    
    RDM6300(uint8_t Data[]);
    uint8_t *_Payload; // used for read comparisons
private:
    static uint8_t ASCIIToByte(uint8_t input);
    enum Operation
    {
        Operation_BeginRead,
        Operation_ReadMSB,
        Operation_ReadLSB,
        Operation_ValidateChecksum,
        Operation_EndRead
    };
private:
    int _cnt;
    Operation _currentOperation;
    uint8_t _temp, _checksum;
};

#endif



