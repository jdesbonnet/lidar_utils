# Livox-SDK
# Confirmed to work on Raspberry Pi 5 / Ubuntu 22.04 host OS
# Remark: add --network=host to the docker run command so that
# the Lidar sensor can be detected on the network.

FROM ubuntu:18.04

# Supress any interactive prompts (eg for TimeZone) during installation
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update

RUN apt install -y lsb-release curl gnupg2 sudo git cmake build-essential

RUN mkdir /opt/livox
WORKDIR /opt/livox

RUN git clone https://github.com/Livox-SDK/Livox-SDK.git
RUN cd Livox-SDK; mkdir build; cd build && cmake ..; make 
#sudo make install

