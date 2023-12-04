cmake_minimum_required(VERSION 3.16)

# Sets project name
project(final_project_playground LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Sets C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specifies required Qt components
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Gui)
find_package(Qt6 REQUIRED COMPONENTS OpenGL)
find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)
find_package(Qt6 REQUIRED COMPONENTS Xml)
find_package(OpenCL REQUIRED)

# Allows you to include files from within those directories, without prefixing their filepaths
include_directories(src)


# Specifies .cpp and .h files to be passed to the compiler
add_executable(${PROJECT_NAME}
    src/main.cpp

    src/realtime.cpp
    src/mainwindow.cpp
    src/settings.cpp
    src/utils/scenefilereader.cpp
    src/utils/sceneparser.cpp

    src/mainwindow.h
    src/realtime.h
    src/settings.h
    src/utils/scenedata.h
    src/utils/scenefilereader.h
    src/utils/sceneparser.h
    src/utils/shaderloader.h
    src/utils/aspectratiowidget/aspectratiowidget.hpp
)


# GLM: this creates its library and allows you to `#include "glm/..."`
add_subdirectory(glm)

# GLEW: this creates its library and allows you to `#include "GL/glew.h"`
add_library(StaticGLEW STATIC glew/src/glew.c
    src/camera.h
    src/camera.cpp
    src/shapes/sphere.cpp
    src/shapes/sphere.h
    src/shapes/cylinder.cpp
    src/shapes/cylinder.h
    src/shapes/cube.cpp
    src/shapes/cube.h
    src/shapes/cone.cpp
    src/shapes/cone.h
    src/ParticleSystem/hello_world.h
    src/ParticleSystem/hello_world.cpp
    src/test.txt
    src/ParticleSystem/ParticleManager.h
    src/ParticleSystem/ParticleManager.cpp
)

#target_compile_definitions(${PROJECT_NAME} PRIVATE USE_PARALLEL_TRANSFORM=1)
#target_compile_options(${PROJECT_NAME} PRIVATE -stdlib=libstdc++)

include_directories(${PROJECT_NAME} PRIVATE glew/include)


# Specifies libraries to be linked (Qt components, glew, etc)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt::Core
    Qt::Gui
    Qt::OpenGL
    Qt::OpenGLWidgets
    Qt::Xml
    StaticGLEW
    ${OpenCL_LIBRARY}
)

include_directories(
    Simple-OpenGL-Image-Library/include/
    tinyobjloader/include/
    tinyxml2/include/
)

add_subdirectory(Simple-OpenGL-Image-Library)

set(TINYXML2_SOURCE
    tinyxml2/src/tinyxml2.cpp
)

set(TINYXML2_HEADERS
        tinyxml2/include/tinyxml2.h
)

include_directories(tinyxml2/include/)

add_library(TINYXML2 STATIC
    ${TINYXML2_SOURCE}
    ${TINYXML2_HEADERS}
)
target_link_libraries(TINYXML2)
set_target_properties(TINYXML2
    PROPERTIES
    PROJECT_LABEL tinyxml2
    FOLDER "Libraries"
)


# Specifies other files
qt6_add_resources(${PROJECT_NAME} "Resources"
    PREFIX
        "/"
    FILES
        resources/shaders/default.frag
        resources/shaders/default.vert
        resources/shaders/texture.frag
        resources/shaders/texture.vert
)

# GLEW: this provides support for Windows (including 64-bit)
if (WIN32)
  add_compile_definitions(GLEW_STATIC)
  target_link_libraries(${PROJECT_NAME} PRIVATE
    opengl32
    glu32
  )
endif()


# Set this flag to silence warnings on Windows
if (MSVC OR MSYS OR MINGW)
  set(CMAKE_CXX_FLAGS "-Wno-volatile")
endif()
# Set this flag to silence warnings on MacOS
if (APPLE)
  set(CMAKE_CXX_FLAGS "-Wno-deprecated-volatile")
endif()
