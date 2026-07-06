cmake_minimum_required(VERSION 3.18)
project(edgevision VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY /lib)

# Compile options
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0 -Wall -Wextra)
else()
    add_compile_options(-O2 -Wall)
endif()

# Find dependencies
find_package(OpenCV REQUIRED COMPONENTS core imgproc videoio highgui)
find_package(SQLite3 REQUIRED)
find_package(Threads REQUIRED)

# Header file directory
include_directories(
    /src
    
    
)

# Collect source files
file(GLOB_RECURSE SOURCS "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

# Executable target
add_executable(edgevision src/main.cpp )

# Link libraries
target_link_libraries(edgevision PRIVATE
    
    
    Threads::Threads
)

# Platform-specific settings
if(WIN32)
    target_link_libraries(edgevision PRIVATE ws2_32)
elseif(UNIX)
    find_library(REALTIME_LIB NAMES rt)
    if(REALTIME_LIB)
        target_link_libraries(edgevision PRIVATE )
    endif()
endif()

# Install rules
install(TARGETS edgevision DESTINATION bin)
install(DIRECTORY config/ DESTINATION config)
install(DIRECTORY models/ DESTINATION models)

# Print configuration info
message(STATUS "========== EdgeVision Build Config ==========")
message(STATUS "  CMake version: ")
message(STATUS "  Build type: ")
message(STATUS "  OpenCV version: ")
message(SQLite3 version: )
message(STATUS "  Source files: ")
message(STATUS "=============================================")
