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
  uint8_t _u8FrontAntPort;
  uint8_t _u8BackAntPort;
  uint8_t _u8FrontAntPwr;
  uint8_t _u8BackAntPwr;
 public:
  RFID();
  int8_t i8Connect(char* pchIpAddr, int iPort);
  uint8_t u8CalculatePowerVal(float fPV);
  bool bConfigFrontAntennaNumber(uint8_t u8ChosenAntenna);
  bool bConfigBackAntennaNumber(uint8_t u8ChosenAntenna);
  bool bConfigFrontAntennaPower(float fPwr);
  bool bConfigBackAntennaPower(float fPwr);
  bool bPrepareForFrontAntennaScan();
  bool bPrepareForBackAntennaScan();

  // fPowerValue: 17dBm-33dBm (with 1/4dBm resolution),
  // (default value: 25dBm)
  // Port: 1-4
  bool bSetAntennaPower(uint8_t u8Port, uint8_t u8PowerValue);
  bool bWriteTagEPC(uint16_t u16NewEPC, uint32_t u32Password);
  bool bScanForTags(std::vector <std::uint16_t>& vDiscoveredTags);
  ~RFID();
};
