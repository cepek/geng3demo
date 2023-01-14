cmake_minimum_required(VERSION 3.5)

project(geng3demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(geng3demo main.cpp)

install(TARGETS geng3demo
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
