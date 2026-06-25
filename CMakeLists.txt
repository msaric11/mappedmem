cmake_minimum_required(VERSION 3.20)
set(CMAKE_BUILD_TYPE Debug)
project(mappedmem)

set(CMAKE_CXX_STANDARD 20)

add_executable(mappedmem src/main.cpp src/proc.cpp)

find_package(fmt REQUIRED)

target_link_libraries(mappedmem PRIVATE fmt::fmt)
