cmake_minimum_required(VERSION 3.15)
project(ethernet_sim LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Catch2 Setup
find_package(Catch2 3 REQUIRED)


add_library(eth STATIC
  src/EthernetFrame.cpp
  src/EthernetDriver.cpp)
target_include_directories(eth PUBLIC include)

# Tests
add_executable(tests
  tests/test_frame.cpp
  tests/test_driver.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain eth)

# Demo 1: Ping/Pong
# add_executable(ethernet_demo src/Demo1.cpp)
# target_link_libraries(ethernet_demo PRIVATE eth)

# Demo 2: Stream

# Demo 3: Error Handling
