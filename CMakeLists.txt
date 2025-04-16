cmake_minimum_required(VERSION 3.0.0)
project(ao-cpp VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-O3")
add_executable(ao-cpp main.cpp scheduler.cpp active_object.cpp future.cpp
        active_object.cpp
        future.h
        cs_base.h
        continuation_scheduler.h
        continuation_scheduler.ipp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
