FROM ros:humble
ENV DEBIAN_FRONTEND=noninteractive
SHELL ["/bin/bash", "-c"]

# 기존 ROS2 repository 제거 후 키 재설정
RUN rm -rf /etc/apt/sources.list.d/ros2*.list \
    && apt-get update && apt-get install -y curl gnupg2 lsb-release \
    && curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg \
    && echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu $(lsb_release -cs) main" | tee /etc/apt/sources.list.d/ros2.list > /dev/null

# 필요한 패키지 설치
RUN apt-get update && apt-get install -y \
    libpcl-dev \
    libeigen3-dev \
    python3-colcon-common-extensions \
    ros-humble-pcl-ros \
    ros-humble-pcl-conversions \
    ros-humble-rviz2 \
    git \
    cmake \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

# Livox SDK 2 설치
RUN cd /tmp \
    && git clone https://github.com/Livox-SDK/Livox-SDK2.git \
    && cd Livox-SDK2 \
    && mkdir build && cd build \
    && cmake .. && make -j$(nproc) \
    && make install \
    && rm -rf /tmp/Livox-SDK2

WORKDIR /workspace
