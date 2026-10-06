cmake_minimum_required(VERSION 3.28)
project(SaturnAntiCheat C)

set(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION "10.0.28000.0")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src/launcher)
add_subdirectory(src/driver)