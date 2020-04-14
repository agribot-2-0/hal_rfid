#pragma once

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#include <vector>

#define CMD_SyncGetEPCs 0x0101
#define RES_SyncGetEPCs 0x8101

class RFID
{
private:
    int _iSocketFd;
    std::vector <std::string>_vDiscoveredTags;
public:
    RFID(char* sIpAddr, int iPort);
    void pchSendFrame(char* pchBuff, uint8_t u8BuffSize);
    void pchRecFrame(char* pchBuff, uint8_t u8BuffSize);
    uint32_t u32GetU32(uint8_t u8StartBit, char* pchMsg);
    uint16_t u16GetU16(uint8_t u8StartBit, char* pchMsg);
    void vParseFrame(char* pchMsg);
    bool bScanForTags();
    std::vector<std::string> vGetIDs();
    ~RFID();
};
