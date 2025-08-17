cmake_minimum_required(VERSION 3.20)

project(kwport LANGUAGES CXX)

file(GLOB KWPORT_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

add_library(kwport INTERFACE)

target_include_directories(kwport INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
