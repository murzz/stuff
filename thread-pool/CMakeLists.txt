cmake_minimum_required(VERSION 2.8)
set(name thread-pool)
project(${name})

find_package(Boost COMPONENTS thread system program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_executable (${name} src/main.cpp)
target_link_libraries(${name} ${Boost_LIBRARIES} glog)

set(CMAKE_BUILD_TYPE Debug)
