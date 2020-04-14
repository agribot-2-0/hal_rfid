#include "hal_rfid/Rfid.h"

#define CMD_SyncGetEPCs 0x0101
#define RES_SyncGetEPCs 0x8101
#define RF_NoError 0x00
#define RF_NoTag 0x0a
#define RF_NoStandard 0x05
#define RF_WrongCommandFunctionMode 0x0E

#define FRAME_SIZE_BYTE 2
#define FRAME_ID_BYTE 6
#define FRAME_DATA_START 8

#define FRAME_START 0x5a50
#define FRAME_END 0xa5

#define BUFF_SIZE 50
#define BYTE_MASK 0xff


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

uint32_t RFID::u32GetU32(uint8_t u8StartBit, char* pchMsg)
{
    return ((uint32_t)pchMsg[u8StartBit+3]) << 24
    | ((uint32_t)pchMsg[u8StartBit+2]) << 16
    | ((uint32_t)pchMsg[u8StartBit+1]) << 8
    | ((uint32_t)pchMsg[u8StartBit]);
}

uint16_t RFID::u16GetU16(uint8_t u8StartBit, char* pchMsg)
{
    return ((uint16_t)pchMsg[u8StartBit+1]) << 8
    | ((uint16_t)pchMsg[u8StartBit]);
}

void RFID::vParseFrame(char* pchMsg)
{
    uint8_t u8FrameSize = 0;
    for (u8FrameSize = 0; u8FrameSize < BUFF_SIZE; u8FrameSize++)
    {
        pchMsg[u8FrameSize] = (uint)pchMsg[u8FrameSize]&BYTE_MASK;
        std::cout << std::hex << (((uint)pchMsg[u8FrameSize]&BYTE_MASK)) << std:: endl;
        if((pchMsg[u8FrameSize]&BYTE_MASK) == FRAME_END)
        {
            break;
        }
    }

    uint32_t u32MsgSize = u32GetU32(FRAME_SIZE_BYTE, pchMsg);
    uint16_t u16MsgId = u16GetU16(FRAME_ID_BYTE, pchMsg);

    for(uint8_t i = FRAME_DATA_START; i < u8FrameSize-2;)
    {
        if(pchMsg[i] == RF_NoError)
        {
            i+=3;
            uint8_t u32NumberOfEPCWords =  pchMsg[i];
            i+=1;
            for (uint8_t j = 1; j <= u32NumberOfEPCWords; j++)
            {
                _vDiscoveredTags.push_back(u16GetU16(i, pchMsg));
                std::cout << "Pushing back: " << std::hex << std:: endl;
                i+=2;
            }
            i+=1;
        }
        if(pchMsg[i] == RF_NoTag)
        {
            _vDiscoveredTags.clear();
            i+=3;
        }
        if(pchMsg[i] == FRAME_END)
        {
            return;
        }
    }
    
    std::cout << std::hex << u32MsgSize << " " << u16MsgId << std:: endl;
}

bool RFID::bScanForTags()
{
    char pchMessage[BUFF_SIZE];
    bzero(pchMessage, (sizeof(char)*BUFF_SIZE));
    // start of frame:
    pchMessage[0] = (FRAME_START&0xff00)>>8;
    pchMessage[1] = (FRAME_START&BYTE_MASK);
    // size - value in bytes (described by 4 bytes, LSB first)
    uint32_t u32MsgSize = 2;
    pchMessage[2] = (u32MsgSize&0xff);
    pchMessage[3] = (u32MsgSize&0xff00)>>8;
    pchMessage[4] = (u32MsgSize&0xff0000)>>16;
    pchMessage[5] = (u32MsgSize&0xff000000)>>24;
    // command id (LSB first):
    pchMessage[6] = CMD_SyncGetEPCs&0x00ff;
    pchMessage[7] = (CMD_SyncGetEPCs&0xff00)>>8;
    // frame end:
    pchMessage[8] = FRAME_END;
    pchSendFrame(pchMessage, 9);
    pchRecFrame(pchMessage, BUFF_SIZE);
    vParseFrame(pchMessage);
}

std::vector<uint16_t> RFID::vGetIDs()
{
    return _vDiscoveredTags;
}

RFID::~RFID()
{
    close(_iSocketFd);
}
