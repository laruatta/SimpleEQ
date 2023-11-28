cmake_minimum_required(VERSION 3.28)
project(SimpleEQ VERSION 1.0)
set(CMAKE_C_STANDARD 23)

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
include(cmake/CPM.cmake)

CPMAddPackage(
    NAME JUCE
    GITHUB_REPOSITORY juce-framework/JUCE
    GIT_TAG 7.0.9
    VERSION 7.0.9
    SOURCE_DIR ${LIB_DIR}/juce
)

add_subdirectory(src)

# include(InstallRequiredSystemLibraries)
# set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
# set(CPACK_PACKAGE_VERSION_MAJOR "${_VERSION_MAJOR}")
# set(CPACK_PACKAGE_VERSION_MINOR "${_VERSION_MINOR}")
# include(CPack)

if(MSVC)
    add_compile_options(/Wall /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()