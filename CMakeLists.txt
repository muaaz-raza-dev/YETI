cmake_minimum_required(VERSION 3.20)

project(YETI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include headers
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add executable
add_executable(app main.cpp)

# Link local curl library
target_link_libraries(app PRIVATE
    ${CMAKE_SOURCE_DIR}/libcurl-x64.dll
)