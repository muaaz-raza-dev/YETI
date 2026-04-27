cmake_minimum_required(VERSION 3.20)

project(YETI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Define the executable
add_executable(app main.cpp)

# 2. Pass ONLY the root directory to C++ (Safely avoids space parsing errors)
target_compile_definitions(app PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}")

# 3. Branch the library linking based on the Operating System
if(WIN32)
   # Point to your new include folder
    target_include_directories(app PRIVATE ${CMAKE_SOURCE_DIR}/include)
    
    # Link against the .lib file in your new lib folder
    # NOTE: Check inside your lib folder to ensure the file is named exactly 'libcurl.lib'
    target_link_libraries(app PRIVATE ${CMAKE_SOURCE_DIR}/lib/libcurl.lib)

    # Automatically copy the DLL from your source folder to the build folder
    add_custom_command(TARGET app POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/libcurl-x64.dll"
        $<TARGET_FILE_DIR:app>
    )
else()
    # Native Ubuntu/Linux configuration
    find_package(CURL REQUIRED)
    include_directories(${CURL_INCLUDE_DIRS})
    target_link_libraries(app PRIVATE CURL::libcurl)
endif()