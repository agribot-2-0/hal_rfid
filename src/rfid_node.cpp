#include "ros/ros.h"
#include "hal_rfid/Rfid.h"
#include "hal_rfid/Tags.h"

#define PORT 4007
#define RFID_IP "192.168.0.2"

#define DEFAULT_ANTENAT_PWR 25 //dBm

RFID RfidHandler(RFID_IP, PORT);

bool bScan(hal_rfid::Tags::Request &req,
           hal_rfid::Tags::Response &res)
{
    if (req.selected_antenna == req.FRONT)
    {
        if ((RfidHandler.bSetAntennaPower(1, DEFAULT_ANTENAT_PWR) != true) ||
            (RfidHandler.bSetAntennaPower(2, 0) != true))
        {
            ROS_ERROR("[RFID] Antenna Configuration failed");
        }
    }
    if (req.selected_antenna == req.BACK)
    {
        if ((RfidHandler.bSetAntennaPower(1, DEFAULT_ANTENAT_PWR) != true) ||
            (RfidHandler.bSetAntennaPower(2, 0) != true))
        {
            ROS_ERROR("[RFID] Antenna Configuration failed");
        }
    }
    return RfidHandler.bScanForTags(res.ids);
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "rfid");
    ros::NodeHandle n;
    ros::ServiceServer Service = 
        n.advertiseService("hal_rfid/tags", bScan);
    // Example of writin new EPC to tag:
    // RfidHandler.bWriteTagEPC(0x1234, 0);
    ros::spin();
    return 0;
}