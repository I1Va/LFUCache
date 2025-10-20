cmake_minimum_required(VERSION 3.10)
project(LFUCache LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option(SANITIZE OFF)

if(SANITIZE)
    message(STATUS "Sanitizers enabled")

    set(SAN_FLAGS "-fsanitize=address,undefined")

    add_compile_options(${SAN_FLAGS})
    add_link_options(${SAN_FLAGS})
else()
    message(STATUS "Sanitizers disabled")
endif()
    

include_directories(${CMAKE_SOURCE_DIR}/inc)

add_subdirectory(tests)

add_executable(LFU
   LFU.cpp
)

add_executable(Belady
    Belady.cpp
)