/*
* Copyright (C) 2020 Agribot. All rights reserved.
*/

#include <ros/ros.h>
#include <agribot_msgs/Tags.h>

int main(int argc, char** argv)
{
    ros::init(argc, argv, "example_rfid_client");
    ros::NodeHandle n;
    ros::ServiceClient client =
        n.serviceClient<agribot_msgs::Tags>("hal_rfid/tags");
    agribot_msgs::Tags test2;
    test2.request.selected_antenna = test2.request.BACK;

    if (client.call(test2))
    {
        ROS_INFO("[RFID CLIENT] Found %d tags:",
                 static_cast<int>(test2.response.ids.size()));
        while (!test2.response.ids.empty())
        {
            ROS_INFO("0x%x", test2.response.ids.back());
            test2.response.ids.pop_back();
        }
    }
    else
    {
        ROS_ERROR("[RFID CLIENT] Error while requesting tags scan");
    }
    return 0;
}
