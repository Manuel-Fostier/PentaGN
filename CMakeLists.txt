cmake_minimum_required(VERSION 3.16)
project(PentaGN LANGUAGES CXX)

# Put the output in the build directory just like the SDL example
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# Use the vendored SDL source tree if present
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

add_executable(penta main.cpp)

# Link against SDL3 from the vendored tree
target_link_libraries(penta PRIVATE SDL3::SDL3)

target_compile_features(penta PRIVATE cxx_std_17)
