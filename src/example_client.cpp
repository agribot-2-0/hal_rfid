#include "ros/ros.h"
#include "hal_rfid/Rfid.h"
#include "hal_rfid/Tags.h"

int main(int argc, char** argv)
{
    ros::init(argc, argv, "example_rfid_client");
    ros::NodeHandle n;
    ros::ServiceClient client = 
        n.serviceClient<hal_rfid::Tags>("hal_rfid/tags");
    hal_rfid::Tags test2;
    test2.request.selected_antenna = test2.request.FRONT;

    if(client.call(test2))
    {
        std::cout << "Found " << test2.response.ids.size() << " tags:" << std:: endl;
        while(!test2.response.ids.empty())
        {
            std::cout << std::hex << test2.response.ids.back() << std::endl;
            test2.response.ids.pop_back();
        }
    }
    else
    {
        std::cout << "ERROR" << std:: endl;
    }

    return 0;
}