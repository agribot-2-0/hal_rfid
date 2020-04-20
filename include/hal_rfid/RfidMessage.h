#pragma once

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include <vector>

// Msgs IDs:
#define CMD_SyncGetEPCs 0x0101
#define CMD_SetPortPower 0x0006
#define CMD_SyncWriteEPCToSingleTag 0x010f
#define RES_SyncGetEPCs 0x8101
#define RES_SetPortPower 0x8006
#define RES_SyncWriteEPCToSingleTag 0x810f

// Return flags:
#define RF_NoError 0x00
#define RF_NoTag 0x0a
#define RF_NoStandard 0x05
#define RF_WrongCommandFunctionMode 0x0e

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
    void vMakeRfidFrame(uint16_t u16Id, uint8_t* pu8Data, uint8_t u8DataLength);
    void vSendFrame(int iSocketFd);
    void vRecFrame(int iSocketFd);
    uint32_t u32GetU32(uint8_t u8StartBit, char* pchMsg);
    uint16_t u16GetU16(uint8_t u8StartBit, char* pchMsg);
    bool bCheckSetPowerResp(uint8_t u8Port);
    bool bCheckWriteEPCResp();
    bool bCheckGetEPCResp(std::vector<std::uint16_t>& vDiscoveredTags);
    ~RfidMessage();
};