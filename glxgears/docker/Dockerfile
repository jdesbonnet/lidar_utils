#FROM ubuntu:18.04
FROM nvidia/opengl:1.2-glvnd-devel-ubuntu18.04


# Supress any interactive prompts (eg for TimeZone) during installation
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update

RUN apt install -y lsb-release curl sudo git cmake x11-apps build-essential
RUN apt install -y mesa-utils

RUN apt-get install -y \
    mesa-utils \
    libgl1-mesa-dri \
    libgl1-mesa-glx \
    x11-apps


# https://gitlab.com/nvidia/container-images/opengl/blob/ubuntu18.04/base/Dockerfile
# Run with ~/bin/docker_x11_run.sh --rm -it --runtime=nvidia  glxgears


