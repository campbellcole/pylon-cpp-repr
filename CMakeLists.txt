cmake_minimum_required(VERSION 3.16)
project(pylon-cpp-repr VERSION 0.1.0)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

message(CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SourceFiles "${CMAKE_CURRENT_LIST_DIR}/src/*")
set(PYLON_CPP_REPR_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/include")
add_executable(pylon-cpp-repr ${SourceFiles})
include_directories(pylon-cpp-repr ${PYLON_CPP_REPR_INCLUDE})

find_package(pylon 7.3.0 REQUIRED)

target_link_libraries(pylon-cpp-repr pylon::pylon)
