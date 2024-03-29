FROM ubuntu:22.04
RUN apt-get update
RUN mkdir /opt/kitware
WORKDIR /opt/kitware

RUN apt-get install -y build-essential byacc flex freeglut3-dev libbz2-dev  libffi-dev libfontconfig1-dev libfreetype6-dev libnl-genl-3-dev libopengl0  libprotobuf-dev libx11-dev libx11-xcb-dev libx11-xcb-dev libxcb-glx0-dev  libxcb-glx0-dev libxcb-icccm4-dev libxcb-image0-dev libxcb-keysyms1-dev  libxcb-randr0-dev libxcb-render-util0-dev libxcb-shape0-dev libxcb-shm0-dev libxcb-sync-dev libxcb-util-dev libxcb-xfixes0-dev libxcb-xinerama0-dev  libxcb-xkb-dev libxcb1-dev libxext-dev libxext-dev libxfixes-dev libxi-dev  libxkbcommon-dev libxkbcommon-dev libxkbcommon-x11-dev libxkbcommon-x11-dev  libxrender-dev libxt-dev pkg-config protobuf-compiler zlib1g-dev

RUN apt-get install -y libglx-dev git cmake ninja-build
RUN apt-get install -y python3 python3-all-dev libtbb-dev mpi-default-dev

# Verified to work with commit 496bfed1b6bf16fd25d51711c87bd32b9e52b862 (HEAD -> master, origin/master, origin/HEAD)
# Tested build on both AMD64 and ARM64 architecture, but was unable to get UI to start on ARM64 from within docker.
RUN git clone https://gitlab.kitware.com/LidarView/paraview

# HEAD (commit 496bfed1b6bf16fd25d51711c87bd32b9e52b862) worked, now trying v5.9.1 - didn't work!
#RUN cd paraview; git checkout v5.9.1; 
RUN cd paraview; git submodule update --init --recursive
RUN mkdir paraview_build

ENV DEBIAN_FRONTEND=noninteractive

RUN apt install -y qtbase5-dev
RUN apt install -y qttools5-dev libqt5svg5-dev

RUN cd paraview_build; cmake -GNinja -DPARAVIEW_USE_PYTHON=ON -DPARAVIEW_USE_MPI=ON -DVTK_SMP_IMPLEMENTATION_TYPE=TBB -DCMAKE_BUILD_TYPE=Release ../paraview

# Got config working, now problems with xmlpatterns
# https://askubuntu.com/questions/1454926/xmlpatterns-could-not-find-a-qt-installation-of
# https://github.com/easybuilders/easybuild-easyconfigs/issues/6302
RUN apt install -y libqt5xmlpatterns5-dev qtxmlpatterns5-dev-tools


RUN git clone https://gitlab.kitware.com/keu-computervision/slam
RUN cd slam; git submodule update --init --recursive
RUN apt install -y libboost-all-dev libnanoflann-dev libeigen3-dev libceres-dev libpcl-dev

RUN mkdir slam_build; cd slam_build; cmake ../slam
RUN cd slam_build; cmake --build . -j

# works!
