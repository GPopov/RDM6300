/*
  RFID RDM6300 data decoder library
  (c) Stephane Driussi 20150623
  Not for commercial use
*/
#include "stdafx.h"
#include "RDM6300.h"

RDM6300::RDM6300(uint8_t Data[])
    :_currentOperation(Operation_BeginRead)
    , _cnt(0)
    , _temp(0)
    {
        _Payload = Data;
    }

bool RDM6300::decode(uint8_t input) {

    if (input == 0x02)
    {
        _cnt = 0;
        _checksum = 0;        
        _temp = 0;
        _currentOperation = Operation_BeginRead;
    }

    switch (_currentOperation)
    {
        case Operation_BeginRead:
            if (input == 0x02) {
                _currentOperation = Operation_ReadMSB;
            }
            break;                    
        case Operation_ReadMSB:
            _temp = ASCIIToByte(input);
            _currentOperation = Operation_ReadLSB;
            break;
        case Operation_ReadLSB:
            input = ASCIIToByte(input);
            _temp = (_temp << 4) | input;
            _Payload[_cnt++] = _temp;
            if (_cnt > 5) {
                _currentOperation = Operation_EndRead;
            }
            else {
                _currentOperation = Operation_ReadMSB;
                _checksum ^= _temp;
            }
            break;
        case Operation_EndRead:
            if (input == 0x03) {
                _currentOperation = Operation_ValidateChecksum;
            }
            else {
                _currentOperation = Operation_BeginRead;
            }
        case Operation_ValidateChecksum:
            break;
    }

    if (_currentOperation == Operation_ValidateChecksum)
    {
        _currentOperation = Operation_BeginRead;
        _cnt = 0;
        if (_checksum == _Payload[5]) {
            return true;
        }
    }
        
    return false;
}

uint8_t RDM6300::ASCIIToByte(uint8_t input)
{
    uint8_t val = input;
    val -= 0x30; //0x30 is '0' in the ASCII table
    if (val > 9)
        val -= 7; //There are 7 symbols between 9 and A in the ASCII table- namely : ; < = > ? @
    return val;
}
