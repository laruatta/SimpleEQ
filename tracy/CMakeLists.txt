cmake_minimum_required(VERSION 3.28)

set(TRACY_ENABLE true)

target_sources(SimpleEQ
    PUBLIC
        TracyClient.cpp)

target_include_directories(SimpleEQ
    PUBLIC
        /home/luke/repos/tracy/public)
