#pragma once

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <hal_rfid/RfidMessage.h>

#include <vector>

class RFID
{
private:
    int _iSocketFd;
public:
    RFID(char* sIpAddr, int iPort);
    void vSendFrame(char* pchBuff, uint8_t u8BuffSize);
    void vRecFrame(char* pchBuff, uint8_t u8BuffSize);
    uint32_t u32GetU32(uint8_t u8StartBit, char* pchMsg);
    uint16_t u16GetU16(uint8_t u8StartBit, char* pchMsg);
    bool bParseGetEPCResp(char* pchMsg, std::vector <std::uint16_t>& vDiscoveredTags);
    bool bParseSetPowerResp(char* pchMsg);
    bool bParseWriteEPCResp(char* pchMsg);
    uint8_t u8CalculatePowerVal(float fPV);
    // fPowerValue: 17dBm-33dBm (with 1/4dBm resolution), 
    // (default value: 25dBm) 
    // Port: 1-4
    bool bSetAntennaPower(uint8_t u8Port, float fPowerValue);
    bool bWriteTagEPC(uint16_t u16NewEPC, uint32_t u32Password);
    bool bScanForTags(std::vector <std::uint16_t>& vDiscoveredTags);
    ~RFID();
};
