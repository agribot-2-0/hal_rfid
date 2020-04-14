#include "ros/ros.h"
#include "hal_rfid/Rfid.h"
#include "hal_rfid/Tags.h"

#define PORT 4007
#define RFID_IP "192.168.0.2"

int main(int argc, char **argv)
{
    ros::init(argc, argv, "rfid");
    ros::NodeHandle n;
    ros::ServiceClient client = 
        n.serviceClient<hal_rfid::Tags>("hal_rfid/tags");
    hal_rfid::Tags xTagsServer;

    RFID test(RFID_IP, PORT);
    test.bScanForTags();
    return 0;
}