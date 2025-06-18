cmake_minimum_required(VERSION 3.15)
project(ethernet_sim LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Packages
find_package(Catch2 3 REQUIRED)
find_package(Threads REQUIRED)

# Create Ethernet library
add_library(eth STATIC
  src/EthernetFrame.cpp
  src/EthernetDriver.cpp
  src/Protocol.cpp
  src/ProtocolHost.cpp
  src/ProtocolDevice.cpp)
target_include_directories(eth PUBLIC include)

# Tests
add_executable(tests
  tests/TestFrame.cpp
  tests/TestDriver.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain eth)

# Demo: Ping/Pong
add_executable(ethernet_demo src/Demo.cpp)
target_link_libraries(ethernet_demo PRIVATE eth Threads::Threads)
