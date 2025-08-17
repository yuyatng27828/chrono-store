cmake_minimum_required(VERSION 3.10)
project(chrono_store)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include header files
include_directories(include)

# Source files
file(GLOB SOURCES "src/*.cpp")

# Executable target
add_executable(chrono_store ${SOURCES})
