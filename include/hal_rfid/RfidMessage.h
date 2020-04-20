#pragma once

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include <vector>

#define CMD_SyncGetEPCs 0x0101
#define CMD_SetPortPower 0x0006
#define CMD_SyncWriteEPCToSingleTag 0x010f
#define RES_SyncGetEPCs 0x8101
#define RES_SetPortPower 0x8006
#define RES_SyncWriteEPCToSingleTag 0x810f

#define RF_NoError 0x00
#define RF_NoTag 0x0a
#define RF_NoStandard 0x05
#define RF_WrongCommandFunctionMode 0x0e

#define FRAME_SIZE_BYTE 2
#define FRAME_ID_BYTE 6
#define FRAME_DATA_START 8

#define FRAME_START 0x5a50
#define FRAME_END 0xa5

#define BUFF_SIZE 50
#define BYTE_MASK 0xff

#define MIN_POWER_DBM 17 //dBm
#define MAX_POWER_DBM 33 //dBm
#define MIN_POWER_HEX 0x44
#define MAX_POWER_HEX 0x84
#define RESOLUTION 0.25

class RfidMessage
{
private:
    char* _pchBuffer;
    uint16_t _u16FrameIt;
    uint16_t _u16BufferSize;
public:
    RfidMessage(uint16_t u16FrameSize);
    uint8_t u8PrintFrame();
    void vSetFrameStart();
    // u32MsgSize - In bytes (id size + data size)
    void vSetFrameLen(uint32_t u32MsgSize);
    void vSetFrameId(uint16_t u16Id);
    void vSetFrameData(uint8_t* pu8Data, uint8_t u8DataLength);
    void vSetFrameEnd();
    void vSendFrame(int iSocketFd);
    void vRecFrame(int iSocketFd);
    uint32_t u32GetU32(uint8_t u8StartBit, char* pchMsg);
    uint16_t u16GetU16(uint8_t u8StartBit, char* pchMsg);
    bool bParseSetPowerResp();
    bool bParseWriteEPCResp();
    bool bParseGetEPCResp(std::vector<std::uint16_t>& vDiscoveredTags);
    char* pchGetFrame();
    uint16_t u16GetMsgLen();
    ~RfidMessage();
};