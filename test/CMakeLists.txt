cmake_minimum_required(VERSION 3.28)

project(SimpleEQTest)

enable_testing()

add_executable(${PROJECT_NAME}
    src/SimpleEQTest.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${GOOGLETEST_SOURCE_DIR}/googletest/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
        ${JUCE_SOURCE_DIR}/modules
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        SimpleEQ
        GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})