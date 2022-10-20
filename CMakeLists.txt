# Quick start guide to building with CMake.
#
# Download, install and add CMake to your PATH ( https://cmake.org/download ).
#
# For Windows, do:
#   cd build
#   cmake .. -G "Visual Studio 10 2010" (for VS2010)
#     or
#   cmake .. -G "Visual Studio 14 2015" (for VS2015)
#   cmake --build . --config Release
#
# For Unix, do:
#   cd build
#   cmake .. -DCMAKE_BUILD_TYPE=Release
#   cmake --build .

cmake_minimum_required(VERSION 3.10)

project(libusbmux)

add_library(${PROJECT_NAME} SHARED
    src/usbmux.c
    usbmux.rc
)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER inc/usbmux.h)

target_include_directories(${PROJECT_NAME} PRIVATE
    inc
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    WIN32
)
