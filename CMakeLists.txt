cmake_minimum_required(VERSION 2.8)
project(depthComp)

IF ( UNIX )
   set( CMAKE_PREFIX_PATH "/opt/opencv" )
   set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)
   MESSAGE( "Using - UNIX/LINUX CONFIG" )
ENDIF ( UNIX )

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp depthComp.cpp)

add_executable(depthComp main.cpp depthComp.cpp)

target_link_libraries (depthComp ${OpenCV_LIBS}
        ${GLUT_LIBRARY}
        ${OPENGL_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        )
