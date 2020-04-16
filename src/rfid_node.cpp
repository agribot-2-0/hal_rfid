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
        test.bSetAntennasPower(1, 25);
        test.bSetAntennasPower(2, 0);
    }
    if (req.selected_antenna == req.BACK)
    {
        std::cout << "BACK" << std::endl;
        test.bSetAntennasPower(1, 0);
        test.bSetAntennasPower(2, 25);
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