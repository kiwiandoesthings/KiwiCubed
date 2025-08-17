cmake_minimum_required(VERSION 3.20)

project(klogger LANGUAGES CXX)

add_library(klogger STATIC
    klogger.cpp
)

target_include_directories(klogger PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
