FROM ubuntu:18.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update
RUN apt-get install -y build-essential byacc flex freeglut3-dev libbz2-dev  libffi-dev libfontconfig1-dev libfreetype6-dev libnl-genl-3-dev libopengl0  libprotobuf-dev libx11-dev libx11-xcb-dev libx11-xcb-dev libxcb-glx0-dev  libxcb-glx0-dev libxcb-icccm4-dev libxcb-image0-dev libxcb-keysyms1-dev  libxcb-randr0-dev libxcb-render-util0-dev libxcb-shape0-dev libxcb-shm0-dev libxcb-sync-dev libxcb-util-dev libxcb-xfixes0-dev libxcb-xinerama0-dev  libxcb-xkb-dev libxcb1-dev libxext-dev libxext-dev libxfixes-dev libxi-dev  libxkbcommon-dev libxkbcommon-dev libxkbcommon-x11-dev libxkbcommon-x11-dev  libxrender-dev libxt-dev pkg-config protobuf-compiler zlib1g-dev
RUN apt-get install -y cmake mesa-utils
RUN apt-get install -y ninja-build
#RUN apt-get install -y libglx-dev not available for 18.04, but available for 20.04+

# glxgears work now

RUN apt install -y lsb-release curl sudo git cmake x11-apps build-essential
RUN sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list'
RUN curl -s https://raw.githubusercontent.com/ros/rosdistro/master/ros.asc | sudo apt-key add -
 
RUN apt-get update
RUN apt install -y ros-melodic-desktop-full  #for ubuntu 18.04
#RUN apt install -y ros-noetic-desktop-full  #for ubuntu 20.04

RUN mkdir -p /opt/livox
WORKDIR /opt/livox
RUN git clone https://github.com/Livox-SDK/livox_ros_driver.git ws_livox/src
COPY build.sh .

# Close?
#roslaunch livox_mapping mapping_horizon.launch
#RLException: [mapping_horizon.launch] is neither a launch file in package [livox_mapping] nor is [livox_mapping] a launch file name
#The traceback for the exception was written to the log file

# Cant get glxgears to work with ubuntu18.04
