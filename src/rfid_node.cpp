#include <ros/ros.h>
#include <hal_rfid/Rfid.h>
#include <agribot_msgs/Tags.h>

#define PORT 4007
#define RFID_IP "192.168.0.2"

#define DEFAULT_ANTENAT_PWR 23  // dBm

RFID RfidHandler(RFID_IP, PORT);

bool bScan(agribot_msgs::Tags::Request &req,
           agribot_msgs::Tags::Response &res)
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
        if ((RfidHandler.bSetAntennaPower(1, 0) != true) ||
            (RfidHandler.bSetAntennaPower(2, DEFAULT_ANTENAT_PWR) != true))
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
    // [Example] Writin new EPC to tag:
    // if(!RfidHandler.bWriteTagEPC(0x123a, 0))
    // {
    //     ROS_ERROR("[RFID] Write EPC failed (probably no tags found)");
    // }
    ros::spin();
    return 0;
}
