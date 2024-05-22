cmake_minimum_required(VERSION 3.28)

option(TRACY_ENABLE "")

# target_link_libraries(SimpleEQ
#     PUBLIC
#         Tracy::TracyClient)

target_sources(SimpleEQ
    PUBLIC
        TracyClient.cpp)

target_include_directories(SimpleEQ
    PUBLIC
        /home/luke/repos/tracy/public)
