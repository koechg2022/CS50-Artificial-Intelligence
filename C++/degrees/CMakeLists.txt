

cmake_minimum_required(VERSION 3.10)

# Set the compiler and point to it
set(CMAKE_C_COMPILER "/usr/local/Cellar/llvm/18.1.8/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/local/Cellar/llvm/18.1.8/bin/clang++")


project(degrees)


# Set the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add warning flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/headers)
include_directories(${PROJECT_SOURCE_DIR}/util)


# Output directories
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/objects)

# Create executables
add_executable(degrees ${PROJECT_SOURCE_DIR}/degrees.c++)

# Set output directory for executables
set_target_properties(degrees PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/objects)