#include "hal_rfid/Rfid.h"

#define CMD_SyncGetEPCs 0x0101
#define CMD_SetPortPower 0x0006
#define RES_SyncGetEPCs 0x8101
#define RES_SetPortPower 0x8006
//SyncWriteEPCSpecyfic

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

void RFID::vParseFrame(char* pchMsg, std::vector<std::uint16_t>& vDiscoveredTags)
{
    vDiscoveredTags.clear();

    uint8_t u8FrameSize = 0;
    for (u8FrameSize = 0; u8FrameSize < BUFF_SIZE; u8FrameSize++)
    {
        std::cout << std::hex << (((uint)pchMsg[u8FrameSize]&BYTE_MASK)) << std:: endl;
        if((pchMsg[u8FrameSize]&BYTE_MASK) == FRAME_END)
        {
            break;
        }
    }

    // Tags EPC frame:
    if(u16GetU16(FRAME_ID_BYTE, pchMsg) == RES_SyncGetEPCs)
    {
        uint8_t u8MsgIt = FRAME_DATA_START;
        while(true)
        {
            if((pchMsg[u8MsgIt]&BYTE_MASK) == RF_NoTag)
            {
                break;
            }
            if((pchMsg[u8MsgIt]&BYTE_MASK) == RF_NoError)
            {
                while(true)
                {
                    u8MsgIt+=4;
                    if(((pchMsg[u8MsgIt-2]&BYTE_MASK) == FRAME_END) ||
                        (u8MsgIt >= BUFF_SIZE))
                       {
                           break;
                       }
                    vDiscoveredTags.push_back(u16GetU16(u8MsgIt, pchMsg));
                }
                break;
            }
            if((pchMsg[u8MsgIt]&BYTE_MASK) == RF_NoStandard |
               (pchMsg[u8MsgIt]&BYTE_MASK) == RF_WrongCommandFunctionMode)
            {
                break;
            }
        }
    }
    return;
}

uint8_t RFID::u8CalculatePowerVal(float fPV)
{
    uint8_t u8Val = 0;
    if((fPV <= MAX_POWER_DBM) && (fPV >= MIN_POWER_DBM))
    {
        u8Val = MIN_POWER_HEX + ((uint8_t)fPV- MIN_POWER_DBM)/RESOLUTION;
    }
    return u8Val;
}

// True if everything went well
bool RFID::bSetAntennasPower(uint8_t u8Port, float fPowerValue)
{
    uint8_t u8PowerVal = u8CalculatePowerVal(fPowerValue);
    char pchMessage[BUFF_SIZE];
    bzero(pchMessage, (sizeof(char)*BUFF_SIZE));
    // start of frame:
    pchMessage[0] = (FRAME_START&0xff00)>>8;
    pchMessage[1] = (FRAME_START&BYTE_MASK);
    // size - value in bytes (described by 4 bytes, LSB first)
    uint32_t u32MsgSize = 4;
    pchMessage[2] = (u32MsgSize&0xff);
    pchMessage[3] = (u32MsgSize&0xff00)>>8;
    pchMessage[4] = (u32MsgSize&0xff0000)>>16;
    pchMessage[5] = (u32MsgSize&0xff000000)>>24;
    // command id (LSB first):
    pchMessage[6] = CMD_SetPortPower&0x00ff;
    pchMessage[7] = (CMD_SetPortPower&0xff00)>>8;
    pchMessage[8] = u8Port;
    pchMessage[9] = u8PowerVal;
    // frame end:
    pchMessage[10] = FRAME_END;
    pchSendFrame(pchMessage, 11);
    // TODO: check response frame, and return false in case of errors
    return true;
}

void RFID::vScanForTags(std::vector<std::uint16_t>& vDiscoveredTags)
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
    vParseFrame(pchMessage, vDiscoveredTags);
    return;
}

RFID::~RFID()
{
    close(_iSocketFd);
}
