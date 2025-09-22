cmake_minimum_required(VERSION 3.16)
project(3DEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2 using homebrew path on macOS
set(SDL2_INCLUDE_DIRS /opt/homebrew/include/SDL2)
set(SDL2_LIBRARIES /opt/homebrew/lib/libSDL2.dylib)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/graphics)
include_directories(${SDL2_INCLUDE_DIRS})

# Source files
set(SOURCES
    src/main.cpp
    graphics/renderer.cpp
)

# Create executable
add_executable(3DEngine ${SOURCES})

# Link libraries
target_link_libraries(3DEngine ${SDL2_LIBRARIES})