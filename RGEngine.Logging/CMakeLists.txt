# CMakeList.txt : CMake project for RGEngine.Logging, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.20.1)

# Add source to this project's executable.
add_executable(RGEngine.Logging "src/logger.c" "includes/RGEngine/logger.h" "main.c")

target_include_directories(RGEngine.Logging PUBLIC "includes")