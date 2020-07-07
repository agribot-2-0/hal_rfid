/*
* Copyright (C) 2020 Agribot. All rights reserved.
*/

#include <hal_rfid/RfidMessage.h>

#define FRAME_SIZE_BYTE 2
#define FRAME_ID_BYTE 6
#define FRAME_DATA_START_BYTE 8

#define EPC_DATA_LEN 4
#define ID_LEN 2

#define FRAME_START 0x5a50
#define FRAME_END 0xa5

#define BYTE_MASK 0xff

// Return flags:
#define RF_NoError 0x00
#define RF_NoTag 0x0a
#define RF_NoStandard 0x05
#define RF_WrongCommandFunctionMode 0x0e

RfidMessage::RfidMessage(uint16_t u16BuffSize)
{
    _u16BufferSize = u16BuffSize;
    _pchBuffer = new char[_u16BufferSize];
    bzero(_pchBuffer, (sizeof(char)*_u16BufferSize));
    _u16FrameIt = FRAME_DATA_START_BYTE;
}

uint8_t RfidMessage::u8PrintFrame()
{
    uint8_t u8FrameSize;
    for (u8FrameSize = 0; u8FrameSize < 40; u8FrameSize++)
    {
        std::cout << std::hex << (((uint)_pchBuffer[u8FrameSize]&BYTE_MASK));
        std::cout << std:: endl;
        if ((_pchBuffer[u8FrameSize]&BYTE_MASK) == FRAME_END)
        {
            break;
        }
    }
    return u8FrameSize;
}

void RfidMessage::vSetFrameStart()
{
    _pchBuffer[0] = (FRAME_START&0xff00)>>8;
    _pchBuffer[1] = (FRAME_START&BYTE_MASK);
}

void RfidMessage::vSetFrameLen(uint32_t u32MsgLen)
{
    _pchBuffer[2] = (u32MsgLen&0xff);
    _pchBuffer[3] = (u32MsgLen&0xff00)>>8;
    _pchBuffer[4] = (u32MsgLen&0xff0000)>>16;
    _pchBuffer[5] = (u32MsgLen&0xff000000)>>24;
}

void RfidMessage::vSetFrameId(uint16_t u16Id)
{
    _pchBuffer[6] = (u16Id&0xff);
    _pchBuffer[7] = (u16Id&0xff00)>>8;
}

void RfidMessage::vSetFrameData(uint8_t* pu8Data, uint8_t u8DataLength)
{
    for (uint8_t i = 0; i < u8DataLength; i++)
    {
        _pchBuffer[_u16FrameIt] = pu8Data[i];
        _u16FrameIt++;
    }
}

void RfidMessage::vSetFrameEnd()
{
    _pchBuffer[_u16FrameIt] = FRAME_END;
}

void RfidMessage::vMakeRfidFrame(uint16_t u16Id, uint8_t* pu8Data,
                                 uint8_t u8DataLength)
{
    vSetFrameStart();
    vSetFrameLen(u8DataLength+ID_LEN);
    vSetFrameId(u16Id);
    vSetFrameData(pu8Data, u8DataLength);
    vSetFrameEnd();
}

void RfidMessage::vSendFrame(int iSocketFd)
{
    write(iSocketFd, _pchBuffer, (sizeof(char)*(_u16FrameIt+1)));
}

void RfidMessage::vRecFrame(int iSocketFd)
{
    bzero(_pchBuffer, (sizeof(char)*_u16BufferSize));
    read(iSocketFd, _pchBuffer, (sizeof(char)*_u16BufferSize));
}

uint32_t RfidMessage::u32GetU32(uint8_t u8StartBit, char* pchMsg)
{
    return ((uint32_t)pchMsg[u8StartBit+3]) << 24
    | ((uint32_t)pchMsg[u8StartBit+2])&BYTE_MASK << 16
    | ((uint32_t)pchMsg[u8StartBit+1])&BYTE_MASK << 8
    | ((uint32_t)pchMsg[u8StartBit])&BYTE_MASK;
}

uint16_t RfidMessage::u16GetU16(uint8_t u8StartBit, char* pchMsg)
{
    return ((uint16_t)pchMsg[u8StartBit+1]) << 8
    | ((uint16_t)pchMsg[u8StartBit])&BYTE_MASK;
}

bool RfidMessage::bCheckSetPowerResp(uint8_t u8Port)
{
    if (u16GetU16(FRAME_ID_BYTE, _pchBuffer) == RES_SetPortPower)
    {
        if ((_pchBuffer[FRAME_DATA_START_BYTE] == RF_NoError) &&
            (_pchBuffer[FRAME_DATA_START_BYTE+1] == u8Port))
        {
            return true;
        }
    }
    return false;
}

bool RfidMessage::bCheckWriteEPCResp()
{
    if (u16GetU16(FRAME_ID_BYTE, _pchBuffer) == RES_SyncWriteEPCToSingleTag)
    {
        if (_pchBuffer[FRAME_DATA_START_BYTE] == RF_NoError)
        {
            return true;
        }
    }
    return false;
}

bool RfidMessage::bCheckGetEPCResp(std::vector<std::uint16_t>& vDiscoveredTags)
{
    vDiscoveredTags.clear();
    // Tags EPC frame:
    if (u16GetU16(FRAME_ID_BYTE, _pchBuffer) == RES_SyncGetEPCs)
    {
        uint8_t u8MsgIt = FRAME_DATA_START_BYTE;
        while (true)
        {
            if ((_pchBuffer[u8MsgIt]&BYTE_MASK) == RF_NoTag)
            {
                return true;
            }
            if ((_pchBuffer[u8MsgIt]&BYTE_MASK) == RF_NoError)
            {
                while (true)
                {
                    u8MsgIt += EPC_DATA_LEN;
                    if (((_pchBuffer[u8MsgIt-2]&BYTE_MASK) == FRAME_END) ||
                         (u8MsgIt >= _u16BufferSize))
                       {
                           break;
                       }
                    vDiscoveredTags.push_back(u16GetU16(u8MsgIt, _pchBuffer));
                }
                return true;
            }
            if ((_pchBuffer[u8MsgIt]&BYTE_MASK) == RF_NoStandard |
                (_pchBuffer[u8MsgIt]&BYTE_MASK) == RF_WrongCommandFunctionMode)
            {
                return false;;
            }
        }
    }
}

RfidMessage::~RfidMessage()
{
    delete[] _pchBuffer;
}
