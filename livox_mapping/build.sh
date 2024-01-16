#!/bin/bash
source /opt/ros/melodic/setup.bash
cd ws_livox && catkin_make
source /opt/livox/ws_livox/devel/setup.sh
catkin_make
source /opt/livox/ws_livox/devel/setup.sh 

