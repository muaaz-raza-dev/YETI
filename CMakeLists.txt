cmake_minimum_required(VERSION 3.15)
project(HttpClient)

set(CMAKE_CXX_STANDARD 17)

# Include headers
include_directories(${CMAKE_SOURCE_DIR}/include)

# Link library path
link_directories(${CMAKE_SOURCE_DIR}/lib)

# Create executable
add_executable(app main.cpp)

# Link libcurl
target_link_libraries(app curl)