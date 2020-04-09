#include "hal_rfid/Rfid.h"

#define PORT 4007
#define RFID_IP "192.168.0.2"

int main()
{
    RFID test(RFID_IP, PORT);
    test.bScanForTags();
    return 0;
}