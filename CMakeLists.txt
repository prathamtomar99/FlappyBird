cmake_minimum_required(VERSION 3.16)
project(FlappyBird LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Tell CMake where to find the keg-only SFML 2.x
# Homebrew on Apple Silicon uses /opt/homebrew
set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/sfml@2")

# 2. Find SFML components (Note: Use lowercase for components in 2.x)
find_package(SFML 2.6.2 COMPONENTS graphics window system REQUIRED)

add_executable(FlappyBird 
    src/main.cpp 
    src/Game.cpp
    src/Bird.cpp
    src/Pipe.cpp
    src/BirdState.cpp
)

# 3. Include your custom headers
target_include_directories(FlappyBird PRIVATE "${CMAKE_SOURCE_DIR}/Header Files")


add_custom_command(
    TARGET FlappyBird POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "${CMAKE_CURRENT_BINARY_DIR}/assets"
)

# 4. Link using the namespaced targets provided by SFML
target_link_libraries(FlappyBird
    PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
)
