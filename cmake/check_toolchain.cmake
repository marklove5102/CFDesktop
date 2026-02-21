# Toolchain Configuration Helper
# Supports two usage modes:
# 1. Direct:  cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain.cmake -S . -B out/build
# 2. Shorthand: cmake -DUSE_TOOLCHAIN=windows/llvm -S . -B out/build

# Check if user has directly specified CMAKE_TOOLCHAIN_FILE
if(DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Toolchain file directly specified:")
    message(STATUS "  ${CMAKE_TOOLCHAIN_FILE}")
    return()
endif()

# User hasn't specified CMAKE_TOOLCHAIN_FILE, check USE_TOOLCHAIN shorthand
if(NOT DEFINED USE_TOOLCHAIN)
    message(FATAL_ERROR
        "No toolchain specified!\n"
        "Use either:\n"
        "  1. Shorthand: cmake -DUSE_TOOLCHAIN=windows/llvm -S . -B out/build\n"
        "  2. Direct:    cmake -DCMAKE_TOOLCHAIN_FILE=\\$PWD/cmake/cmake_toolchain/windows/llvm-toolchain.cmake -S . -B out/build"
    )
endif()

# Parse USE_TOOLCHAIN format: platform/toolchain
# Examples: windows/llvm, windows/msvc, linux/gcc
string(FIND "${USE_TOOLCHAIN}" "/" SEPARATOR_POS)

if("${SEPARATOR_POS}" STREQUAL "-1")
    message(FATAL_ERROR "Invalid USE_TOOLCHAIN format: '${USE_TOOLCHAIN}'. Expected format: platform/toolchain (e.g., windows/llvm)")
endif()

# Extract platform and toolchain config
string(SUBSTRING "${USE_TOOLCHAIN}" 0 ${SEPARATOR_POS} PLATFORM)
string(LENGTH "${USE_TOOLCHAIN}" TOTAL_LENGTH)
math(EXPR CONFIG_START "${SEPARATOR_POS} + 1")
string(SUBSTRING "${USE_TOOLCHAIN}" ${CONFIG_START} -1 TOOLCHAIN_CONFIG)

# Construct toolchain file path
# Supports both:
# - windows/llvm -> cmake/cmake_toolchain/windows/llvm-toolchain.cmake
# - windows/llvm-toolchain -> cmake/cmake_toolchain/windows/llvm-toolchain.cmake
if("${TOOLCHAIN_CONFIG}" MATCHES "-toolchain$")
    set(TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_toolchain/${PLATFORM}/${TOOLCHAIN_CONFIG}.cmake")
else()
    set(TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_toolchain/${PLATFORM}/${TOOLCHAIN_CONFIG}-toolchain.cmake")
endif()

# Convert to absolute path
get_filename_component(TOOLCHAIN_FILE "${TOOLCHAIN_FILE}" ABSOLUTE)

# Check if toolchain file exists
if(NOT EXISTS "${TOOLCHAIN_FILE}")
    message(FATAL_ERROR "Toolchain file not found: ${TOOLCHAIN_FILE}")
endif()

# Set CMAKE_TOOLCHAIN_FILE for CMake to use
set(CMAKE_TOOLCHAIN_FILE "${TOOLCHAIN_FILE}" CACHE FILEPATH "Toolchain file" FORCE)

message(STATUS "Toolchain shorthand '${USE_TOOLCHAIN}' resolved to:")
message(STATUS "  ${TOOLCHAIN_FILE}")
