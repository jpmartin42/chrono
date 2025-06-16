ARG ROS_DISTRO
ARG USERHOME
ARG USERSHELLPROFILE
ARG ROS_INSTALL_PREFIX
ARG ROS_WORKSPACE_DIR="${USERHOME}/packages/workspace"
ARG CHRONO_ROS_INTERFACES_DIR="${ROS_WORKSPACE_DIR}/src/chrono_ros_interfaces"

ARG CUSTOM_WORKSPACE_DIR="${USERHOME}/packages/wheel_limb_robotics"

# Install relevant and required packages
RUN sudo apt update && \
        sudo apt install --no-install-recommends -y \
        liborocos-kdl-dev \
        libeigen3-dev \
        ros-humble-ign-ros2-control