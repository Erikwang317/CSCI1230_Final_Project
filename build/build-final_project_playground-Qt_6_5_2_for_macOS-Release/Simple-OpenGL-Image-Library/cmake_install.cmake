# Install script for directory: /Users/charleston/Documents/Brown/Courses/Computer Graphics/final_project_playground/Simple-OpenGL-Image-Library

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

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/charleston/Documents/Brown/Courses/Computer Graphics/final_project_playground/build/build-final_project_playground-Qt_6_5_2_for_macOS-Release/Simple-OpenGL-Image-Library/libSOIL.a")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSOIL.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSOIL.a")
    execute_process(COMMAND "/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSOIL.a")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SOIL" TYPE FILE FILES
    "/Users/charleston/Documents/Brown/Courses/Computer Graphics/final_project_playground/Simple-OpenGL-Image-Library/include/SOIL.h"
    "/Users/charleston/Documents/Brown/Courses/Computer Graphics/final_project_playground/Simple-OpenGL-Image-Library/include/image_DXT.h"
    "/Users/charleston/Documents/Brown/Courses/Computer Graphics/final_project_playground/Simple-OpenGL-Image-Library/include/image_helper.h"
    "/Users/charleston/Documents/Brown/Courses/Computer Graphics/final_project_playground/Simple-OpenGL-Image-Library/include/stb_image_aug.h"
    "/Users/charleston/Documents/Brown/Courses/Computer Graphics/final_project_playground/Simple-OpenGL-Image-Library/include/stbi_DDS_aug.h"
    "/Users/charleston/Documents/Brown/Courses/Computer Graphics/final_project_playground/Simple-OpenGL-Image-Library/include/stbi_DDS_aug_c.h"
    )
endif()

