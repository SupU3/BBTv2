# Add source files
file(GLOB_RECURSE SOURCES "*.cpp")

# Add the executable
add_executable(BBTServer ${SOURCES})

# Link dependencies (example with Boost)
# find_package(Boost REQUIRED)
target_link_libraries(BBTServer PRIVATE Boost::asio Boost::system Boost::thread)

# Add other dependencies similarly
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")