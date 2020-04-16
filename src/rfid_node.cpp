#include "ros/ros.h"
#include "hal_rfid/Rfid.h"
#include "hal_rfid/Tags.h"

#define PORT 4007
#define RFID_IP "192.168.0.2"

RFID test(RFID_IP, PORT);

bool bScan(hal_rfid::Tags::Request &req,
           hal_rfid::Tags::Response &res)
{
    if (req.selected_antenna == req.FRONT)
    {
        std::cout << "FRONT" << std::endl;
        if ((test.bSetAntennaPower(1, 25) != true) || 
            (test.bSetAntennaPower(2, 0) != true))
        {
            ROS_ERROR("[RFID] Antenna Configuration failed");
        }
    }
    if (req.selected_antenna == req.BACK)
    {
        std::cout << "BACK" << std::endl;
        if ((test.bSetAntennaPower(1, 25) != true) || 
            (test.bSetAntennaPower(2, 0) != true))
        {
            ROS_ERROR("[RFID] Antenna Configuration failed");
        }
    }
    test.vScanForTags(res.ids);
    return true;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "rfid");
    ros::NodeHandle n;
    //ros::ServiceClient client = 
    //    n.serviceClient<hal_rfid::Tags>("hal_rfid/tags");
    ros::ServiceServer Service = 
        n.advertiseService("hal_rfid/tags", bScan);
    ros::spin();
    return 0;
}