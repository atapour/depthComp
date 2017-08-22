cmake_minimum_required(VERSION 2.8)
project(holeFilling)

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp
        deepFill.cpp)

add_executable(holeFilling main.cpp deepFill.cpp)

target_link_libraries (holeFilling ${OpenCV_LIBS}
        ${GLUT_LIBRARY}
        ${OPENGL_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        )

