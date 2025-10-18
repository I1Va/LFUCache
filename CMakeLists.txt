cmake_minimum_required(VERSION 3.10)
project(LFUCache LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O3")


include_directories(${CMAKE_SOURCE_DIR}/inc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined -fno-omit-frame-pointer")
set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address,undefined")



add_subdirectory(tests)

add_executable(LFU
   LFU.cpp
)


add_executable(Belady
    Belady.cpp
)