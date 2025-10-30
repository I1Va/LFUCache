cmake_minimum_required(VERSION 3.20)
project(LFUCache LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

option(SANITIZE "Enable compiler sanitizers" OFF)
option(BUILD_TESTS "Build unit tests" ON)

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(
        -Wall
        -Wextra
        -Werror
         $<$<BOOL:${SANITIZE}>:-fsanitize=address,undefined>
    )
    add_link_options(
         $<$<BOOL:${SANITIZE}>:-fsanitize=address,undefined>
    )
endif()
    
add_executable(LFU LFU.cpp)
add_executable(Belady Belady.cpp)

target_include_directories(LFU PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_include_directories(Belady PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()
