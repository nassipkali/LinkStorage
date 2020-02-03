cmake_minimum_required(VERSION 3.5)

project(LinksPlatform LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(LinksPlatform main.cpp)

add_subdirectory(Links)

target_link_libraries(LinksPlatform Links)
