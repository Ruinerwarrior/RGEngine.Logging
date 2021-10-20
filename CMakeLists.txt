# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("RGEngine.Logging" C)

set (CMAKE_C_STANDARD 99)

option(BUILD_TESTS "Build unit tests" OFF)

include(CTest)
enable_testing()

# Include sub-projects.
add_subdirectory ("RGEngine.Logging")

#if(BUILD_TESTS)
#add_subdirectory ("Tests")
#endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
