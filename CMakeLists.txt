cmake_minimum_required(VERSION 2.6.0)

if (POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
endif()

project(SurfelMapping LANGUAGES CXX VERSION 0.1)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/Pangolin/build/src")

set(SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/Shaders" CACHE PATH "Where the shaders live")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -O0 -std=c++11 -DSHADER_DIR=${SHADER_DIR}")
set(CMAKE_BUILD_TYPE Debug)

find_package(Pangolin 0.1 REQUIRED PATHS ${CMAKE_MODULE_PATH})
find_package(OpenCV REQUIRED)

message("!!! CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH})
message("!!!Pangolin location: " ${Pangolin_INCLUDE_DIRS} ${Pangolin_CMAKEMODULES})


set(Python3_ROOT_DIR "/opt/miniconda3/envs/surfel")
set(Python3_FIND_STRATEGY "LOCATION")
find_package(Python3 3.8 COMPONENTS Interpreter Development REQUIRED)
message(STATUS "PYTHON3_EXECUTABLE: ${Python3_EXECUTABLE}")

# code for lib
include_directories(
        ${OpenCV_INCLUDE_DIRS}
        ${Pangolin_INCLUDE_DIRS}
        ${EIGEN_INCLUDE_DIRS}
        src
        src/Utils
)

file(GLOB srcs CONFIGURE_DEPENDS src/*.cpp)
file(GLOB utils_srcs CONFIGURE_DEPENDS src/Utils/*.cpp)

# code for gui
include_directories(
        ${Pangolin_INCLUDE_DIRS}
        ${EIGEN_INCLUDE_DIRS}
        ${OpenCV_INCLUDE_DIR}
        gui
)

file(GLOB gui_srcs CONFIGURE_DEPENDS gui/*.cpp)

add_library(surfelmapping SHARED
        ${srcs}
        ${utils_srcs}
        )

target_link_libraries(surfelmapping
        ${Eigen_LIBRARIES}
        ${Pangolin_LIBRARIES}
        )

add_library(surmap_gui SHARED
        ${gui_srcs}
        )

target_link_libraries(surmap_gui
        ${OpenCV_LIBS}
        ${Eigen_LIBRARIES}
        ${Pangolin_LIBRARIES}
        )

###################### build in c++ ######################
add_executable(build_map build_map.cpp)
target_link_libraries(build_map
        PRIVATE surmap_gui
        PRIVATE surfelmapping
        )
#
add_executable(load_map load_map.cpp)
target_link_libraries(load_map
        PRIVATE surmap_gui
        PRIVATE surfelmapping
        )
##########################################################

###################### build in pybing11 ######################
add_subdirectory(pybind11)

pybind11_add_module(build_map_py build_map_py.cpp)
target_link_libraries(build_map_py
        PRIVATE surmap_gui
        PRIVATE surfelmapping
        )

pybind11_add_module(load_map_py load_map_py.cpp)
target_link_libraries(load_map_py
        PRIVATE surmap_gui
        PRIVATE surfelmapping
        )
##############################################################
