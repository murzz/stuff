cmake_minimum_required(VERSION 2.8)

project(coil)

add_definitions("-std=c++0x -std=c++11")

find_package(Boost COMPONENTS system program_options unit_test_framework REQUIRED)

add_executable(coil src/coil.cpp)
target_link_libraries(coil ${Boost_LIBRARIES})


include(CTest)
add_executable(tests src/tests.cpp)
target_link_libraries(tests ${Boost_LIBRARIES} curl)
add_test(tests tests)