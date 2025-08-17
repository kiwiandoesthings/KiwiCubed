cmake_minimum_required(VERSION 3.20)
project(kiwicubed LANGUAGES CXX)

# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add compile definitions (like Meson's add_project_arguments)
add_compile_definitions(
    GLM_ENABLE_EXPERIMENTAL
    STB_IMAGE_IMPLEMENTATION
)

# Debug flag (equivalent of -g)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g)
endif()

# Handle MSVC-specific flags
if (MSVC)
    add_link_options(/ignore:4099)
endif()

# Collect all source files automatically
file(GLOB_RECURSE KIWICUBED_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp
)

# Add subprojects (treating them as libraries you control)
add_subdirectory(Libs/glad)
add_subdirectory(Libs/klogger)
add_subdirectory(Libs/kwport)

# External dependencies via CMake's find_package (already provided by nix)
find_package(nlohmann_json REQUIRED)
find_package(glfw3 REQUIRED)
find_package(imgui REQUIRED)
find_package(glm REQUIRED)

# Create executable
add_executable(kiwicubed ${KIWICUBED_SOURCES})

# Recursively include all Source/* subdirectories
file(GLOB_RECURSE KIWI_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/*
)
set(SRC_INCLUDE_DIR_LIST "")
foreach(path ${KIWI_INCLUDE_DIRS})
    get_filename_component(dir ${path} DIRECTORY)
    list(APPEND SRC_INCLUDE_DIR_LIST ${dir})
endforeach()
list(REMOVE_DUPLICATES SRC_INCLUDE_DIR_LIST)

target_include_directories(kiwicubed PRIVATE
    ${SRC_INCLUDE_DIR_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/Libs/glad
)


# Link libraries
target_link_libraries(kiwicubed PRIVATE
    kwport
    klogger
    nlohmann_json::nlohmann_json
    glfw
    imgui::imgui
    glm::glm
    glad
)

# Install executable
install(TARGETS kiwicubed RUNTIME DESTINATION bin)
