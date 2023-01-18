cmake_minimum_required(VERSION 3.5)

project(geng3demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(.)

add_executable(geng3demo main.cpp geng3.h geng3.cpp help.cpp demo.cpp exec.cpp
    gnu_gama/ellipsoid.cpp gnu_gama/ellipsoid.h gnu_gama/radian.h
    gnu_gama/intfloat.h gnu_gama/gon2deg.h gnu_gama/gon2deg.cpp
)

install(TARGETS geng3demo
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
