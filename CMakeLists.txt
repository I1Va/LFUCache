cmake_minimum_required(VERSION 3.10)
project(LIRSCache LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O3")

add_subdirectory(tests)

set(SOURCES
   main.cpp
   src/LIRSCache.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/inc)
add_executable(${PROJECT_NAME} ${SOURCES})
