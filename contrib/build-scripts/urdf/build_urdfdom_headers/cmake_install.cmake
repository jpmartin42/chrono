# Install script for directory: /home/josh/wheel_limb_robotics/simulation/chrono/chrono/contrib/build-scripts/urdf/download_urdf/urdfdom_headers

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/home/josh/miniforge3/envs/chrono/bin/x86_64-conda-linux-gnu-objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/urdfdom_headers" TYPE DIRECTORY FILES "/home/josh/wheel_limb_robotics/simulation/chrono/chrono/contrib/build-scripts/urdf/download_urdf/urdfdom_headers/include/")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "cmake" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/urdfdom_headers/cmake" TYPE FILE FILES
    "/home/josh/wheel_limb_robotics/simulation/chrono/chrono/contrib/build-scripts/urdf/build_urdfdom_headers/urdfdom_headers-config.cmake"
    "/home/josh/wheel_limb_robotics/simulation/chrono/chrono/contrib/build-scripts/urdf/build_urdfdom_headers/urdfdom_headers-config-version.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "pkgconfig" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/josh/wheel_limb_robotics/simulation/chrono/chrono/contrib/build-scripts/urdf/build_urdfdom_headers/urdfdom_headers.pc")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/urdfdom_headers/cmake/urdfdom_headersExport.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/urdfdom_headers/cmake/urdfdom_headersExport.cmake"
         "/home/josh/wheel_limb_robotics/simulation/chrono/chrono/contrib/build-scripts/urdf/build_urdfdom_headers/CMakeFiles/Export/128adc559a428c09614b4e4365b8bb09/urdfdom_headersExport.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/urdfdom_headers/cmake/urdfdom_headersExport-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/urdfdom_headers/cmake/urdfdom_headersExport.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/urdfdom_headers/cmake" TYPE FILE FILES "/home/josh/wheel_limb_robotics/simulation/chrono/chrono/contrib/build-scripts/urdf/build_urdfdom_headers/CMakeFiles/Export/128adc559a428c09614b4e4365b8bb09/urdfdom_headersExport.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/josh/wheel_limb_robotics/simulation/chrono/chrono/contrib/build-scripts/urdf/build_urdfdom_headers/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
