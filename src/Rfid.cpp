#include "hal_rfid/Rfid.h"

// Antenna constants:
#define MIN_POWER_DBM 17  // dBm
#define MAX_POWER_DBM 33  // dBm
#define MIN_POWER_HEX 0x44
#define MAX_POWER_HEX 0x84
#define RESOLUTION 0.25
#define MIN_ANT_PORT 1
#define MAX_ANT_PORT 4

#define BUFF_SIZE 50

#define ANTENNA_POWER_OFF 0

RFID::RFID()
{
    _iSocketFd = 0;
    _u8FrontAntPort = 0;
    _u8BackAntPort = 0;
    _u8FrontAntPwr = 0;
    _u8BackAntPwr = 0;
}

int8_t RFID::i8Connect(char* pchIpAddr, int iPort)
{
    struct sockaddr_in xServaddr;
    _iSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_iSocketFd < 0)
    {
        return TCP_SOCK_ERROR;
    }
    bzero(&xServaddr, sizeof(xServaddr));
    xServaddr.sin_family = AF_INET;
    xServaddr.sin_addr.s_addr = inet_addr(pchIpAddr);
    xServaddr.sin_port = htons(iPort);
    if (connect(_iSocketFd, (struct sockaddr*)&xServaddr,
                sizeof(xServaddr)) != 0)
    {
        return RFID_CONN_ERROR;
    }
}

uint8_t RFID::u8CalculatePowerVal(float fPV)
{
    return (uint8_t)(MIN_POWER_HEX + ((uint8_t)fPV - MIN_POWER_DBM)/RESOLUTION);
}

bool RFID::bConfigFrontAntennaNumber(uint8_t u8ChosenAntenna)
{
    if ((u8ChosenAntenna <= MAX_ANT_PORT) &&
    (u8ChosenAntenna >= MIN_ANT_PORT))
    {
        _u8FrontAntPort = u8ChosenAntenna;
        return true;
    }
    return false;
}

bool RFID::bConfigBackAntennaNumber(uint8_t u8ChosenAntenna)
{
    if ((u8ChosenAntenna <= MAX_ANT_PORT) &&
        (u8ChosenAntenna >= MIN_ANT_PORT))
    {
        _u8BackAntPort = u8ChosenAntenna;
        return true;
    }
    return false;
}

bool RFID::bConfigFrontAntennaPower(float fPwr)
{
    if ((fPwr <= MAX_POWER_DBM) && (fPwr >= MIN_POWER_DBM))
    {
        _u8FrontAntPwr = u8CalculatePowerVal(fPwr);
        return true;
    }
    return false;
}

bool RFID::bConfigBackAntennaPower(float fPwr)
{
    if ((fPwr <= MAX_POWER_DBM) && (fPwr >= MIN_POWER_DBM))
    {
        _u8BackAntPwr = u8CalculatePowerVal(fPwr);
        return true;
    }
    return false;
}

bool RFID::bPrepareForFrontAntennaScan()
{
    if (!bSetAntennaPower(_u8FrontAntPort, _u8FrontAntPwr) ||
        !bSetAntennaPower(_u8BackAntPort, ANTENNA_POWER_OFF))
    {
        return false;
    }
    return true;
}

bool RFID::bPrepareForBackAntennaScan()
{
    if (!bSetAntennaPower(_u8FrontAntPort, ANTENNA_POWER_OFF) ||
        !bSetAntennaPower(_u8BackAntPort, _u8BackAntPwr))
    {
        return false;
    }
    return true;
}

bool RFID::bSetAntennaPower(uint8_t u8Port, uint8_t u8PowerValue)
{
    uint8_t u8PowerVal = ANTENNA_POWER_OFF;
    if (u8PowerValue != ANTENNA_POWER_OFF)
    {
        if (u8Port == _u8FrontAntPort)
        {
            u8PowerVal = _u8FrontAntPwr;
        }
        if (u8Port == _u8BackAntPort)
        {
            u8PowerVal = _u8BackAntPwr;
        }
    }
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
