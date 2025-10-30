cmake_minimum_required(VERSION 3.14)
project(TopDownSurvival)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT DEFINED VCPKG_ROOT)
    set(VCPKG_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../vcpkg")
endif()

# Find SFML
find_package(SFML COMPONENTS System Window Graphics CONFIG REQUIRED)

# Add source files
set(SOURCES
    src/main.cpp
    src/game.cpp
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Link SFML imported targets (SFMLConfig provides SFML::System etc.)
target_link_libraries(${PROJECT_NAME} PRIVATE
    SFML::System
    SFML::Window
    SFML::Graphics)