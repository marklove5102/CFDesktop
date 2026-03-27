# ============================================================
# CFDesktop Meta Information Generation
# ============================================================
# This module generates header files containing project meta information
# such as version, build info, and feature configuration.
#
# Output directory: ${CMAKE_BINARY_DIR}/autogen/metainfo/
#
# Usage:
#   1. Include this file early in CMakeLists.txt (after project())
#   2. Call cf_generate_meta_info() after all dependencies are found
# ============================================================

log_module_start("MetaInfo")

# Set output directory for generated header files
set(CFDESKTOP_META_INFO_DIR ${CMAKE_BINARY_DIR}/autogen/metainfo)

# Ensure output directory exists
file(MAKE_DIRECTORY ${CFDESKTOP_META_INFO_DIR})

# ============================================================
# Additional meta variables (can be customized)
# ============================================================

# Repository URL (derived from HOMEPAGE_URL if not set)
set(PROJECT_REPOSITORY_URL "${PROJECT_HOMEPAGE_URL}")

# Author information (can be customized)
set(PROJECT_AUTHOR "Awesome Embedded Learning Studio")

# ============================================================
# Get current date and time
# ============================================================

string(TIMESTAMP BUILD_DATE "%Y-%m-%d")
string(TIMESTAMP BUILD_TIME "%H:%M:%S")

# ============================================================
# Compiler detection
# ============================================================

if(MSVC)
    set(BUILD_COMPILER_NAME "MSVC")
    set(BUILD_COMPILER_VERSION "${MSVC_VERSION}")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(BUILD_COMPILER_NAME "GCC")
    set(BUILD_COMPILER_VERSION "${CMAKE_CXX_COMPILER_VERSION}")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(BUILD_COMPILER_NAME "Clang")
    set(BUILD_COMPILER_VERSION "${CMAKE_CXX_COMPILER_VERSION}")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
    set(BUILD_COMPILER_NAME "AppleClang")
    set(BUILD_COMPILER_VERSION "${CMAKE_CXX_COMPILER_VERSION}")
else()
    set(BUILD_COMPILER_NAME "Unknown")
    set(BUILD_COMPILER_VERSION "Unknown")
endif()

set(BUILD_COMPILER_ID "${CMAKE_CXX_COMPILER_ID}")

# Get C++ standard (with fallback)
if(CMAKE_CXX_STANDARD)
    set(BUILD_CXX_STANDARD "${CMAKE_CXX_STANDARD}")
else()
    set(BUILD_CXX_STANDARD "17")
endif()

# ============================================================
# Build type and platform information
# ============================================================

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(BUILD_TYPE "DEBUG")
elseif(CMAKE_BUILD_TYPE MATCHES "Release")
    set(BUILD_TYPE "RELEASE")
elseif(CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
    set(BUILD_TYPE "RELWITHDEBINFO")
elseif(CMAKE_BUILD_TYPE MATCHES "MinSizeRel")
    set(BUILD_TYPE "MINSIZEREL")
else()
    set(BUILD_TYPE "UNKNOWN")
endif()

set(BUILD_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}")
set(BUILD_SYSTEM_PROCESSOR "${CMAKE_SYSTEM_PROCESSOR}")

# ============================================================
# Function to generate meta info headers
# Call this after find_package(Qt6) is called
# ============================================================

function(cf_generate_meta_info)
    # Get Qt version if available
    if(TARGET Qt6::Core)
        get_target_property(QT_VERSION_EXPORT Qt6::Core VERSION)
        if(QT_VERSION_EXPORT)
            set(QT_VERSION "${QT_VERSION_EXPORT}")
            # Parse version string (format: "6.x.y")
            string(REPLACE "." ";" QT_VERSION_LIST "${QT_VERSION}")
            list(GET QT_VERSION_LIST 0 QT_VERSION_MAJOR)
            list(GET QT_VERSION_LIST 1 QT_VERSION_MINOR)
            list(GET QT_VERSION_LIST 2 QT_VERSION_PATCH)
        else()
            set(QT_VERSION "Unknown")
            set(QT_VERSION_MAJOR "0")
            set(QT_VERSION_MINOR "0")
            set(QT_VERSION_PATCH "0")
        endif()
    else()
        set(QT_VERSION "Unknown")
        set(QT_VERSION_MAJOR "0")
        set(QT_VERSION_MINOR "0")
        set(QT_VERSION_PATCH "0")
    endif()

    # Generate project_info.h
    configure_file(
        ${CMAKE_SOURCE_DIR}/cmake/meta_info/project_info.h.in
        ${CFDESKTOP_META_INFO_DIR}/project_info.h
        @ONLY
    )

    # Generate build_info.h
    configure_file(
        ${CMAKE_SOURCE_DIR}/cmake/meta_info/build_info.h.in
        ${CFDESKTOP_META_INFO_DIR}/build_info.h
        @ONLY
    )

    # Generate feature_config.h (needs Qt version info)
    configure_file(
        ${CMAKE_SOURCE_DIR}/cmake/meta_info/feature_config.h.in
        ${CFDESKTOP_META_INFO_DIR}/feature_config.h
        @ONLY
    )

    log_info("MetaInfo" "Generated meta info headers in: ${CFDESKTOP_META_INFO_DIR}")
endfunction()

log_module_end("MetaInfo")
