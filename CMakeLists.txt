cmake_minimum_required(VERSION 3.5)

project(geng3demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(gamag3/lib/)

add_executable(geng3demo
    main.cpp geng3.h geng3.cpp
    gamag3/lib/gnu_gama/ellipsoid.cpp
    gamag3/lib/gnu_gama/ellipsoid.h
    gamag3/lib/gnu_gama/radian.h
    gamag3/lib/gnu_gama/intfloat.h
    gamag3/lib/gnu_gama/gon2deg.h
    gamag3/lib/gnu_gama/gon2deg.cpp
)
#target_link_libraries(geng3demo ${CMAKE_SOURCE_DIR}/gama/lib/libgama.a)

install(TARGETS geng3demo
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
