#include <ros/ros.h>
#include <hal_rfid/Rfid.h>
#include <agribot_msgs/Tags.h>

int main(int argc, char** argv)
{
    ros::init(argc, argv, "example_rfid_client");
    ros::NodeHandle n;
    ros::ServiceClient client = 
        n.serviceClient<agribot_msgs::Tags>("hal_rfid/tags");
    agribot_msgs::Tags test2;
    test2.request.selected_antenna = test2.request.BACK;

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