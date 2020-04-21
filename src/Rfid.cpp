#include "hal_rfid/Rfid.h"

// Antenna power set constants:
#define MIN_POWER_DBM 17  // dBm
#define MAX_POWER_DBM 33  // dBm
#define MIN_POWER_HEX 0x44
#define MAX_POWER_HEX 0x84
#define RESOLUTION 0.25

#define BUFF_SIZE 50

RFID::RFID(char* pchIpAddr, int iPort)
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
    xServaddr.sin_addr.s_addr = inet_addr(pchIpAddr);
    xServaddr.sin_port = htons(iPort);
    if (connect(_iSocketFd, (struct sockaddr*)&xServaddr,
                sizeof(xServaddr)) != 0)
    {
        std::cerr << "Error while connecting to RFID." << std::endl;
        return;
    }
}

uint8_t RFID::u8CalculatePowerVal(float fPV)
{
    uint8_t u8Val = 0;
    if ((fPV <= MAX_POWER_DBM) && (fPV >= MIN_POWER_DBM))
    {
        u8Val = MIN_POWER_HEX + ((uint8_t)fPV- MIN_POWER_DBM)/RESOLUTION;
    }
    return u8Val;
}

bool RFID::bSetAntennaPower(uint8_t u8Port, float fPowerValue)
{
    uint8_t u8PowerVal = u8CalculatePowerVal(fPowerValue);
    RfidMessage xMsg(BUFF_SIZE);
    uint8_t pu8Data[2] = {u8Port, u8PowerVal};
    xMsg.vMakeRfidFrame(CMD_SetPortPower, pu8Data, 2);
    xMsg.vSendFrame(_iSocketFd);
    xMsg.vRecFrame(_iSocketFd);
    return xMsg.bCheckSetPowerResp(u8Port);
}

bool RFID::bWriteTagEPC(uint16_t u16NewEPC, uint32_t u32Password)
{
    RfidMessage xMsg(BUFF_SIZE);
    uint8_t pu8Data[7];
    // 4 bytes of access password:
    pu8Data[0] = (u32Password&0xff);
    pu8Data[1] = (u32Password&0xff00)>>8;
    pu8Data[2] = (u32Password&0xff0000)>>16;
    pu8Data[3] = (u32Password&0xff000000)>>24;
    // number of EPC words:
    pu8Data[4] = 0x01;
    // new EPC:
    pu8Data[5] = (u16NewEPC&0xff);
    pu8Data[6] = (u16NewEPC&0xff00)>>8;
    xMsg.vMakeRfidFrame(CMD_SyncWriteEPCToSingleTag, pu8Data, 7);
    xMsg.vSendFrame(_iSocketFd);
    xMsg.vRecFrame(_iSocketFd);
    return xMsg.bCheckWriteEPCResp();
}

bool RFID::bScanForTags(std::vector<std::uint16_t>& vDiscoveredTags)
{
    RfidMessage xMsg(BUFF_SIZE);
    uint8_t pu8Data[0];
    xMsg.vMakeRfidFrame(CMD_SyncGetEPCs, pu8Data, 0);
    xMsg.vSendFrame(_iSocketFd);
    xMsg.vRecFrame(_iSocketFd);
    return xMsg.bCheckGetEPCResp(vDiscoveredTags);
}

RFID::~RFID()
{
    close(_iSocketFd);
}
