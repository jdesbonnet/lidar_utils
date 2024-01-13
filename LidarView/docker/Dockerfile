FROM ubuntu:22.04
RUN apt-get update
RUN mkdir /opt/kitware
WORKDIR /opt/kitware

RUN apt-get install -y build-essential byacc flex freeglut3-dev libbz2-dev  libffi-dev libfontconfig1-dev libfreetype6-dev libnl-genl-3-dev libopengl0  libprotobuf-dev libx11-dev libx11-xcb-dev libx11-xcb-dev libxcb-glx0-dev  libxcb-glx0-dev libxcb-icccm4-dev libxcb-image0-dev libxcb-keysyms1-dev  libxcb-randr0-dev libxcb-render-util0-dev libxcb-shape0-dev libxcb-shm0-dev libxcb-sync-dev libxcb-util-dev libxcb-xfixes0-dev libxcb-xinerama0-dev  libxcb-xkb-dev libxcb1-dev libxext-dev libxext-dev libxfixes-dev libxi-dev  libxkbcommon-dev libxkbcommon-dev libxkbcommon-x11-dev libxkbcommon-x11-dev  libxrender-dev libxt-dev pkg-config protobuf-compiler zlib1g-dev

RUN apt-get install -y libglx-dev git cmake ninja-build

RUN git clone --recursive https://gitlab.kitware.com/LidarView/lidarview-superbuild.git lvsb

RUN mkdir build && cd build; cmake ../lvsb -GNinja -DCMAKE_BUILD_TYPE=Release -DENABLE_slam=True -DENABLE_hesaisdk=True; cmake --build . -j

 

