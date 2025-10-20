cmake_minimum_required(VERSION 3.10)
project(LFUCache LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/inc)

add_subdirectory(tests)

add_executable(LFU
   LFU.cpp
)

add_executable(Belady
    Belady.cpp
)