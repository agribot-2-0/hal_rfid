#############################################################################
## Set minimum required version of cmake, project name and compile options ##
#############################################################################
cmake_minimum_required(VERSION 2.8.3)
project(hal_rfid)

#############################################################################
## Find catkin packages and libraries for catkin and system dependencies   ##
#############################################################################
find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  message_generation
)

################################################
## Declare ROS messages, services and actions ##
################################################
add_service_files(
  FILES
  Tags.srv
)

generate_messages(DEPENDENCIES std_msgs)

################################################
## Declare ROS dynamic reconfigure parameters ##
################################################

###################################
## catkin specific configuration ##
###################################
catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES can
#  CATKIN_DEPENDS other_catkin_pkg
#  DEPENDS system_lib
)

#############################################################################
## Build                                                                   ##
#############################################################################
include_directories(
 include
 ${catkin_INCLUDE_DIRS}
)
link_directories("${PROJECT_SOURCE_DIR}")
add_executable(hal_rfid_node src/Rfid.cpp src/RfidMessage.cpp src/rfid_node.cpp)
target_link_libraries(hal_rfid_node ${catkin_LIBRARIES})
add_dependencies(hal_rfid_node ${catkin_EXPORTED_TARGETS} ${${PROJECT_NAME}_EXPORTED_TARGETS})

add_executable(example_client src/example_client.cpp)
target_link_libraries(example_client ${catkin_LIBRARIES})
add_dependencies(example_client ${catkin_EXPORTED_TARGETS} ${${PROJECT_NAME}_EXPORTED_TARGETS})

#############
## Install ##
#############

#############
## Testing ##
#############
