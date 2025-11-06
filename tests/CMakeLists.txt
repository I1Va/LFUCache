enable_testing()

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(UnitTesting Test.cpp)
target_include_directories(UnitTesting PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../inc)
target_include_directories(UnitTesting PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_link_libraries(UnitTesting  PRIVATE GTest::gtest_main)
gtest_discover_tests(UnitTesting)

find_program(PYTHON_EXECUTABLE python3 REQUIRED)
add_test(NAME e2eTestLFU
        COMMAND ${PYTHON_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/e2eTester.py
            "$<TARGET_FILE:LFU>"
            ${CMAKE_CURRENT_SOURCE_DIR}/e2e/LFU
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
add_test(NAME e2eTestBelady
        COMMAND ${PYTHON_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/e2eTester.py
            "$<TARGET_FILE:Belady>"
            ${CMAKE_CURRENT_SOURCE_DIR}/e2e/Belady
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)