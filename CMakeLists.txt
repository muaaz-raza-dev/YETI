cmake_minimum_required(VERSION 3.20)

project(YETI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Define the executable
add_executable(app main.cpp)

# 2. Pass ONLY the root directory to C++ (Safely avoids space parsing errors)
target_compile_definitions(app PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}")

# 3. Branch the library linking based on the Operating System
if(WIN32)
    # Windows configuration
    include_directories(${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(app PRIVATE ${CMAKE_SOURCE_DIR}/libcurl-x64.dll)
else()
    # Native Ubuntu/Linux configuration
    find_package(CURL REQUIRED)
    include_directories(${CURL_INCLUDE_DIRS})
    target_link_libraries(app PRIVATE CURL::libcurl)
endif()