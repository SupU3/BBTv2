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
add_subdirectory(deps/loguru)

set(BOOST_INCLUDE_LIBRARIES asio system thread)
set(BOOST_ENABLE_CMAKE ON)
add_subdirectory(deps/boost EXCLUDE_FROM_ALL)

# Add the executable
add_executable(BBT ${SOURCES})

# Link dependencies (example with Boost)
# find_package(Boost REQUIRED)
target_link_libraries(BBT PRIVATE SDL3::SDL3 opengl32.lib shell32.lib loguru::loguru Boost::asio Boost::system Boost::thread)

# Add other dependencies similarly
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")