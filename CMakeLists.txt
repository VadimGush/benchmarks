cmake_minimum_required(VERSION 3.21)
project(benchmarks)

set(CMAKE_CXX_STANDARD 20)

# For MacOS
include_directories(/usr/local/include)
# For my utilities library
include_directories(utilities/include)

add_executable(step_sum source/step.cpp)
target_compile_definitions(step_sum PRIVATE SUM)
target_compile_definitions(step_sum PRIVATE POLLUTE)

add_executable(step_update source/step.cpp)
target_compile_definitions(step_update PRIVATE UPDATE)
target_compile_definitions(step_update PRIVATE POLLUTE)

add_executable(throughput source/throughput.cpp)