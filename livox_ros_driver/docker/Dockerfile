FROM ubuntu:18.04

# Supress any interactive prompts (eg for TimeZone) during installation
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update

RUN apt install -y lsb-release curl sudo git cmake x11-apps build-essential

RUN sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list'
RUN curl -s https://raw.githubusercontent.com/ros/rosdistro/master/ros.asc | sudo apt-key add -

RUN apt-get update
RUN apt install -y ros-melodic-desktop-full  #for ubuntu 18.04
#RUN apt install -y ros-noetic-desktop-full  #for ubuntu 20.04
#RUN apt install -y ros-desktop-full  #for ubuntu 18.04

RUN mkdir /opt/livox
WORKDIR /opt/livox
RUN git clone https://github.com/Livox-SDK/livox_ros_driver.git ws_livox/src

# Needed to source a file
#RUN rm /bin/sh && ln -s /bin/bash /bin/sh
SHELL ["/bin/bash", "-c"]

RUN source /opt/ros/melodic/setup.sh; cd ws_livox; catkin_make

# To run:
# source ws_livox/devel/setup.sh; roslaunch livox_ros_driver livox_lidar_rviz.launch
# Above works in full VM, but so far no luck in docker container probably due to
# graphics driver issues.
# Error: rviz::RenderSystem: error creating render window: OGRE EXCEPTION(3:RenderingAPIException): Unable to create a suitable GLXContext in GLXContext::GLXContext at /build/ogre-1.9-B6QkmW/ogre-1.9-1.9.0+dfsg1/RenderSystems/GL/src/GLX/OgreGLXContext.cpp (line 61)
# Discussions on above error:
# https://github.com/NVIDIA/nvidia-docker/issues/1438
# https://github.com/elephantrobotics/mycobot_ros/issues/40
# https://forum.hello-robot.com/t/problems-running-rviz-while-using-ssh/67
# Tried adding --runtime=nvidia - no joy

