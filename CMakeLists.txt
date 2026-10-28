cmake_minimum_required(VERSION 3.30)
project(scan VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(target scan)

add_library(${target} INTERFACE)

target_include_directories(${target} INTERFACE include/)
target_compile_features(${target} INTERFACE cxx_std_${CMAKE_CXX_STANDARD})

if(PROJECT_IS_TOP_LEVEL)
    enable_testing()
    add_subdirectory(tests)
endif()
