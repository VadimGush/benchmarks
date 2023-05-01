cmake_minimum_required(VERSION 3.25)
project(benchmarks)

set(CMAKE_CXX_STANDARD 20)

# For MacOS
include_directories(/usr/local/include)
# For my utilities library
include_directories(utilities/include)

add_executable(step source/step.cpp)