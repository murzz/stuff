cmake_minimum_required(VERSION 2.8)
set(name thread-pool)
project(${name})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  # using Clang
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # using GCC
  set(PROJECT_CXX_FLAGS  "-std=c++1y -fsanitize=address -fno-omit-frame-pointer")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  # using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # using Visual Studio C++
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PROJECT_CXX_FLAGS}")

add_definitions(-DBOOST_ALL_DYN_LINK)

include(${CMAKE_CURRENT_SOURCE_DIR}/mk/glog.cmake)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/mk/cmake-modules)

find_package(Boost COMPONENTS thread system program_options timer REQUIRED)
find_package(GLog REQUIRED)

set( ${name}_INCLUDE_DIRS
    ${Boost_INCLUDE_DIRS}
    ${GLOG_INCLUDE_DIRS}
)

set( ${name}_LIBRARIES
    ${Boost_LIBRARIES}
    ${GLOG_LIBRARIES}
)

include_directories(${${name}_INCLUDE_DIRS})

add_executable(${name} src/main.cpp)
target_link_libraries(${name} ${${name}_LIBRARIES} )
