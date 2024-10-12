

cmake_minimum_required(VERSION 3.10)


if (APPLE)
    # Set the compiler and point to it
    

    # find_program(CLANG_C_COMPILER NAMES clang HINTS /usr/local/Cellar/llvm)
    # find_program(CLANG_CXX_COMPILER NAMES clang++ HINTS /usr/local/Cellar/llvm)

    # set(CMAKE_C_COMPILER "/usr/local/Cellar/llvm/*/bin/clang")
    # set(CMAKE_CXX_COMPILER "/usr/local/Cellar/llvm/*/bin/clang++")
    # set(CMAKE_C_COMPILER ${CLANG_C_COMPILER})
    # set(CMAKE_CXX_COMPILER ${CLANG_CXX_COMPILER})

    # Set the base path for LLVM
    set(LLVM_BASE_DIR "/usr/local/Cellar/llvm")

    # Attempt to find the latest version of LLVM
    file(GLOB LLVM_VERSIONS "${LLVM_BASE_DIR}/[0-9]*")

    # If versions are found, use the latest one
    if(LLVM_VERSIONS)
        list(SORT LLVM_VERSIONS)
        list(GET LLVM_VERSIONS -1 LLVM_LATEST)

        # Set the compilers to the latest version of LLVM
        set(CMAKE_C_COMPILER "${LLVM_LATEST}/bin/clang")
        set(CMAKE_CXX_COMPILER "${LLVM_LATEST}/bin/clang++")

        message(STATUS "Using Clang from: ${LLVM_LATEST}")
    else()
        message(FATAL_ERROR "LLVM not found in ${LLVM_BASE_DIR}!")
    endif()


elseif (UNIX AND NOT APPLE)

    # Set the base path for LLVM on Linux
    set(LLVM_BASE_DIR "/usr/lib/llvm")

    # Attempt to find the latest version of LLVM
    file(GLOB LLVM_VERSIONS "${LLVM_BASE_DIR}/[0-9]*")

    # If versions are found, use the latest one
    if(LLVM_VERSIONS)
        list(SORT LLVM_VERSIONS)
        list(GET LLVM_VERSIONS -1 LLVM_LATEST)

        # Set the compilers to the latest version of LLVM
        set(CMAKE_C_COMPILER "${LLVM_LATEST}/bin/clang")
        set(CMAKE_CXX_COMPILER "${LLVM_LATEST}/bin/clang++")

        message(STATUS "Using Clang from: ${LLVM_LATEST}")
    else()
        message(FATAL_ERROR "LLVM not found in ${LLVM_BASE_DIR}!")
    endif()

elseif (WIN32)

    # The windows branch

endif()


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