cmake_minimum_required(VERSION 3.10)
project(BBT)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")

# Include directories
include_directories(src)


add_subdirectory(deps/SDL)

# Add the executable
add_executable(BBT ${SOURCES})

# Link dependencies (example with Boost)
# find_package(Boost REQUIRED)
target_link_libraries(BBT PRIVATE SDL3::SDL3)

# Add other dependencies similarly