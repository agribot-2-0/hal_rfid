#include "hal_rfid/Rfid.h"

RFID::RFID(char* sIpAddr, int iPort)
{
    struct sockaddr_in xServaddr;
    _iSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_iSocketFd < 0)
    {
        std::cerr << "Error while creating TCP socket." << std::endl;
        return;
    }
    bzero(&xServaddr, sizeof(xServaddr));
    xServaddr.sin_family = AF_INET;
    xServaddr.sin_addr.s_addr = inet_addr(sIpAddr);
    xServaddr.sin_port = htons(iPort);
    if (connect(_iSocketFd, (struct sockaddr*)&xServaddr, sizeof(xServaddr)) != 0)
    {
        std::cerr << "Error while connecting to RFID." << std::endl;
        return;
    }
}

void RFID::pchSendFrame(char* pchBuff, uint8_t u8BuffSize) 
{
    write(_iSocketFd, pchBuff, (sizeof(char)*u8BuffSize));
}

void RFID::pchRecFrame(char* pchBuff, uint8_t u8BuffSize)
{
    bzero(pchBuff, (sizeof(char)*u8BuffSize));
    read(_iSocketFd, pchBuff, (sizeof(char)*u8BuffSize));
}

bool RFID::bScanForTags()
{
    char pchMessage[50];
    bzero(pchMessage, (sizeof(char)*50));
    // start of frame:
    pchMessage[0] = 0x5a;
    pchMessage[1] = 0x50;
    // size - value in bytes (described by 4 bytes, LSB first)
    pchMessage[2] = 0x02;
    pchMessage[3] = 0x00;
    pchMessage[4] = 0x00;
    pchMessage[5] = 0x00;
    // command id (LSB first):
    pchMessage[6] = CMD_SyncGetEPCs&0x00ff;
    pchMessage[7] = (CMD_SyncGetEPCs&0xff00)>>8;
    // frame end:
    pchMessage[8] = 0xa5;
    pchSendFrame(pchMessage, 9);
    pchRecFrame(pchMessage, 50);
    for (int i = 0; i < 15; i++)
        std::cout << std::hex << (uint)pchMessage[i] << std:: endl;

}

RFID::~RFID()
{
    close(_iSocketFd);
}
