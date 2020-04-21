#pragma once

#include <hal_rfid/RfidMessage.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <memory>

#define TCP_SOCK_ERROR -1
#define RFID_CONN_ERROR -2

class RFID
{
 private:
    int _iSocketFd;
 public:
    RFID(char* pchIpAddr, int iPort, int8_t *pi8Err);
    uint8_t u8CalculatePowerVal(float fPV);
    // fPowerValue: 17dBm-33dBm (with 1/4dBm resolution),
    // (default value: 25dBm)
    // Port: 1-4
    bool bSetAntennaPower(uint8_t u8Port, float fPowerValue);
    bool bWriteTagEPC(uint16_t u16NewEPC, uint32_t u32Password);
    bool bScanForTags(std::vector <std::uint16_t>& vDiscoveredTags);
    ~RFID();
};
