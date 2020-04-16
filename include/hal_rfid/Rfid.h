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
    void vParseEPCFrame(char* pchMsg, std::vector <std::uint16_t>& vDiscoveredTags);
    uint8_t u8CalculatePowerVal(float fPV);
    // PowerValue: 17dBm-33dBm (with 1/4dBm resolution), 
    // (default value: 25dBm -> 107) 
    // Port: 1-4
    bool bSetAntennaPower(uint8_t u8Port, float fPowerValue);
    void vScanForTags(std::vector <std::uint16_t>& vDiscoveredTags);
    ~RFID();
};
