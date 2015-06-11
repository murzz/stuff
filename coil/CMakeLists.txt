cmake_minimum_required(VERSION 2.8)

project(coil)

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

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PROJECT_CXX_FLAGS}" )

add_definitions(-DDISABLE_LONG_TESTS)
add_definitions(-Wall)
add_definitions(-DBOOST_ALL_DYN_LINK)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/mk/cmake-modules")

find_package(Boost COMPONENTS system program_options unit_test_framework thread date_time log REQUIRED)
find_package(CURL REQUIRED)
find_package(HtmlCxx REQUIRED)

set( COIL_INCLUDE_DIRS
    ${CURL_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${HTMLCXX_INCLUDE_DIRS}
)

set( COIL_LIBRARIES
    ${Boost_LIBRARIES}
    ${CURL_LIBRARIES}
    ${HTMLCXX_LIBRARIES}
)

include_directories(${COIL_INCLUDE_DIRS})

add_executable(coil src/coil.cpp)
target_link_libraries(coil ${COIL_LIBRARIES})

include(CTest)
add_executable(tests src/tests.cpp)
target_link_libraries(tests ${COIL_LIBRARIES})
add_test(tests tests)