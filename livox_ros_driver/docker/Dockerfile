# This base image is suitably configured to run accelerated graphics within
# the docker container (tested by running glxgears). I was not able to do 
# this with ubuntu:18.04 image. Unclear why.
# Source dockerfile for the base image here:
# https://gitlab.com/nvidia/container-images/opengl/blob/ubuntu18.04/base/Dockerfile
#
# Start container using:
# docker run --env="DISPLAY" --volume="/tmp/.X11-unix:/tmp/.X11-unix" \
# --volume="$HOME/.Xauthority:/root/.Xauthority:rw" \
# --runtime=nvidia --rm -it livox_ros_drive
#
# You may need to do 'xhost +' on the host OS to allow X11 connections.
#
# Run using:
# source ws_livox/devel/setup.sh; roslaunch livox_ros_driver livox_lidar_rviz.launch
# 
# Documentation:
# https://github.com/Livox-SDK/livox_ros_driver for documentation.
#

FROM nvidia/opengl:1.2-glvnd-devel-ubuntu18.04


# Supress any interactive prompts (eg for TimeZone) during installation
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update

RUN apt install -y lsb-release curl gnupg2 sudo git cmake x11-apps build-essential

# Needed to install a specific version of ROS. Default Ubuntu ROS does not seem to work.
RUN sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list'
RUN curl -s https://raw.githubusercontent.com/ros/rosdistro/master/ros.asc | sudo apt-key add -

RUN apt-get update
RUN apt install -y ros-melodic-desktop-full  #for ubuntu 18.04
#RUN apt install -y ros-noetic-desktop-full  #for ubuntu 20.04

# Alternative - use default Ubuntu 18.04 packages??
# Default ubuntu 18.04 ROS package. Can't get this to fully build. Getting
# stuck on pcl-ros
#RUN apt install -y ros-desktop-full
#RUN apt install -y libtf2-sensor-msgs-dev librosbag-dev libstd-srvs-dev
#RUN apt install -y ??pcl-ros??

RUN mkdir /opt/livox
WORKDIR /opt/livox
RUN git clone https://github.com/Livox-SDK/livox_ros_driver.git ws_livox/src

# Needed to 'source' a file
SHELL ["/bin/bash", "-c"]

RUN source /opt/ros/melodic/setup.sh; cd ws_livox; catkin_make

