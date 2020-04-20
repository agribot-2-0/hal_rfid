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
bool RFID::bSetAntennaPower(uint8_t u8Port, float fPowerValue)
{
    uint8_t u8PowerVal = u8CalculatePowerVal(fPowerValue);
    RfidMessage xMsg(BUFF_SIZE);
    xMsg.vSetFrameStart();
    xMsg.vSetFrameLen(4);
    xMsg.vSetFrameId(CMD_SetPortPower);
    uint8_t pu8Data[2] = {u8Port, u8PowerVal};
    xMsg.vSetFrameData(pu8Data, 2);
    xMsg.vSetFrameEnd();
    xMsg.vSendFrame(_iSocketFd);
    xMsg.vRecFrame(_iSocketFd);
    // TODO: check response frame, and return false in case of errors
    return xMsg.bParseSetPowerResp();
}

// True if everything went well
bool RFID::bWriteTagEPC(uint16_t u16NewEPC, uint32_t u32Password)
{

    RfidMessage xMsg(BUFF_SIZE);
    xMsg.vSetFrameStart();
    xMsg.vSetFrameLen(9);
    xMsg.vSetFrameId(CMD_SyncWriteEPCToSingleTag);
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
    xMsg.vSetFrameData(pu8Data, 7);
    xMsg.vSetFrameEnd();
    xMsg.vSendFrame(_iSocketFd);
    xMsg.vRecFrame(_iSocketFd);
    // TODO: check response frame, and return
    return xMsg.bParseWriteEPCResp();
}

bool RFID::bScanForTags(std::vector<std::uint16_t>& vDiscoveredTags)
{
    RfidMessage xMsg(BUFF_SIZE);
    xMsg.vSetFrameStart();
    xMsg.vSetFrameLen(2);
    xMsg.vSetFrameId(CMD_SyncGetEPCs);
    xMsg.vSetFrameEnd();
    xMsg.vSendFrame(_iSocketFd);
    xMsg.vRecFrame(_iSocketFd);
    return xMsg.bParseGetEPCResp(vDiscoveredTags);
}

RFID::~RFID()
{
    close(_iSocketFd);
}
