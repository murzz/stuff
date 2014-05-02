cmake_minimum_required(VERSION 2.8)

project(coil)

add_definitions("-std=c++0x -std=c++11")

find_package(Boost COMPONENTS system program_options unit_test_framework thread REQUIRED)
find_package(CURL REQUIRED)

set( COIL_INCLUDE_DIRS
    ${CURL_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

set( COIL_LIBRARIES
    ${Boost_LIBRARIES}
    ${CURL_LIBRARIES}
    htmlcxx
)

include_directories(${COIL_INCLUDE_DIRS})

add_executable(coil src/coil.cpp)
target_link_libraries(coil ${COIL_LIBRARIES})

include(CTest)
add_executable(tests src/tests.cpp)
target_link_libraries(tests ${COIL_LIBRARIES})
add_test(tests tests)