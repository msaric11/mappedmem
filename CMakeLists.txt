cmake_minimum_required(VERSION 3.20)
set(CMAKE_BUILD_TYPE Debug)
project(mappedmem)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

fetchcontent_declare(
    hello_imgui
    GIT_REPOSITORY https://github.com/pthom/hello_imgui.git
    GIT_TAG master
)

fetchcontent_makeavailable(hello_imgui)

find_package(fmt REQUIRED)

add_executable(mappedmem src/main.cpp src/proc.cpp src/app.cpp)


target_link_libraries(mappedmem PRIVATE fmt::fmt hello_imgui)

