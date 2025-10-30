enable_testing()

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(UnitTesting Test.cpp)

target_include_directories(UnitTesting PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../inc)
target_include_directories(UnitTesting PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_link_libraries(UnitTesting  PRIVATE GTest::gtest_main)

gtest_discover_tests(UnitTesting)