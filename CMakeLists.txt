cmake_minimum_required(VERSION 2.8)

project(stuff)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  # using Clang
  set(LOCAL_CXX_FLAGS "-std=c++1y -stdlib=libc++")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # using GCC
  set(LOCAL_CXX_FLAGS "-std=c++1y -fsanitize=address -fno-omit-frame-pointer")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  # using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # using Visual Studio C++
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LOCAL_CXX_FLAGS}")

add_subdirectory(coil)
add_subdirectory(thread-pool)
add_subdirectory(triangle)
