#pragma once

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

class RfidMessage
{
private:
    char* _pchMessage;
    uint16_t _u16FrameIt;
public:
    RfidMessage(uint16_t u16FrameSize);
    uint8_t u8PrintFrame();
    void vSetFrameStart();
    // u32MsgSize - In bytes (id size + data size)
    void vSetFrameLen(uint32_t u32MsgSize);
    void vSetFrameId(uint16_t u16Id);
    void vSetFrameData(uint8_t* pu8Data, uint8_t u8DataLength);
    void vSetFrameEnd();
    char* pchGetFrame();
    uint16_t u16GetMsgLen();
    ~RfidMessage();
};