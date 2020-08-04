/*
 * Copyright (C) 2020 Agribot. All rights reserved.
 */

#include <ros/ros.h>
#include <hal_rfid/Rfid.h>
#include <agribot_msgs/Tags.h>
#include <agribot_msgs/RfidPower.h>

// Default arguments:
// rosrun hal_rfid hal_rfid_node 192.168.0.2 4007 1 22 2 22

RFID RfidHandler;

bool bScan(agribot_msgs::Tags::Request& req,   // NOLINT(runtime/references)
           agribot_msgs::Tags::Response& res)  // NOLINT(runtime/references)
{
  if (req.selected_antenna == req.FRONT)
  {
    if (!RfidHandler.bPrepareForFrontAntennaScan())
    {
      ROS_ERROR("[RFID] Front antenna scan failed");
      return false;
    }
  }
  if (req.selected_antenna == req.BACK)
  {
    if (!RfidHandler.bPrepareForBackAntennaScan())
    {
      ROS_ERROR("[RFID] Back antenna scan failed");
      return false;
    }
  }
  return RfidHandler.bScanForTags(res.ids);
}

bool bSetPower(agribot_msgs::RfidPower::Request& req,   // NOLINT(runtime/references)
               agribot_msgs::RfidPower::Response& res)  // NOLINT(runtime/references)
{
  res.result = res.SUCCESS;
  switch (req.selected_antenna)
  {
    case req.FRONT:
      if (!RfidHandler.bConfigFrontAntennaPower(req.power_value))
      {
        res.result = res.FAIL;
        return false;
      }
      break;
    case req.BACK:
      if (!RfidHandler.bConfigBackAntennaPower(req.power_value))
      {
        res.result = res.FAIL;
        return false;
      }
      break;
    default:
      res.result = res.FAIL;
  }
  return true;
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "rfid");
  ros::NodeHandle n;
  if (argc < 7)
  {
    ROS_ERROR("[RFID] No arguments specified. Typical node call: \n"
              "hal_rfid hal_rfid_node [reader_ip] [reader_tcp_port]"
              " [front_anntena_number] [front_anntena_pwr]"
              " [back_anntena_number] [back_anntena_pwr]");
    return -1;
  }

  int8_t i8ErrorCheck = 0;
  i8ErrorCheck = RfidHandler.i8Connect(argv[1], atoi(argv[2]));
  if (i8ErrorCheck == TCP_SOCK_ERROR || i8ErrorCheck == RFID_CONN_ERROR)
  {
    ROS_ERROR("[RFID] Error while creating RFID object: %d", i8ErrorCheck);
    return -2;
  }
  if (!RfidHandler.bConfigFrontAntennaNumber(atoi(argv[3])) || !RfidHandler.bConfigBackAntennaNumber(atoi(argv[5])))
  {
    ROS_ERROR("[RFID] Allowed antenna port should be chosen from"
              " the interval: [1,4]");
    return -3;
  }
  if (!RfidHandler.bConfigFrontAntennaPower(atof(argv[4])) || !RfidHandler.bConfigBackAntennaPower(atof(argv[6])))
  {
    ROS_ERROR("[RFID] Allowed power should should be chosen from"
              " the interval: [17-33] [dBm] (resolution = 0.25)");
    return -4;
  }
  ROS_INFO("[RFID] Connected to %s on port %d", argv[1], atoi(argv[2]));
  ROS_INFO("[RFID] Front antenna config: %d, %f", atoi(argv[3]), atof(argv[4]));
  ROS_INFO("[RFID] Back antenna config: %d, %f", atoi(argv[5]), atof(argv[6]));

  ros::ServiceServer ServiceTags = n.advertiseService("agribot/rfid/tags", bScan);
  ros::ServiceServer ServiceAntennas = n.advertiseService("agribot/rfid/antennas", bSetPower);

  // [Example] Writin new EPC to tag:
  // if(!RfidHandler.bWriteTagEPC(0x123a, 0))
  // {
  //     ROS_ERROR("[RFID] Write EPC failed (probably no tags found)");
  // }
  ros::spin();
  return 0;
}
