# =============================================================================
# Third Party Dependencies Helper
# =============================================================================
#
# Provides a generic mechanism to download and configure third-party libraries
# to the project's third_party directory.
#
# Usage:
#   setup_third_party(
#       NAME           <library_name>
#       GIT_REPOSITORY <git_url>
#       GIT_TAG        <tag_or_commit>
#       [EXCLUDE_FROM_ALL]
#   )
#
# Parameters:
#   NAME           - Unique name for the third-party library
#   GIT_REPOSITORY - Git repository URL
#   GIT_TAG        - Git tag, branch, or commit hash
#   EXCLUDE_FROM_ALL - (Optional) Exclude this target from 'all' target
#
# Behavior:
#   - If third_party/<NAME> exists and contains CMakeLists.txt:
#       Uses existing source, prints "Already Download And Skip Download"
#   - If third_party/<NAME> does not exist or lacks CMakeLists.txt:
#       Downloads to build/_deps/<name>_src, prints "No Third Party Source Found,
#       Download <NAME>"
#   - On configuration error: exits with FATAL_ERROR
#
# Example:
#   setup_third_party(
#       NAME           googletest
#       GIT_REPOSITORY https://github.com/google/googletest.git
#       GIT_TAG        v1.14.0
#   )
#
# =============================================================================

include(FetchContent)

# =============================================================================
# Function: setup_third_party
# =============================================================================
function(setup_third_party)
    # Parse arguments
    cmake_parse_arguments(
        ARG
        "EXCLUDE_FROM_ALL"    # Options
        "NAME;GIT_REPOSITORY;GIT_TAG"  # Single-value arguments
        ""                     # Multi-value arguments
        ${ARGN}
    )

    # Validate required arguments
    if(NOT ARG_NAME)
        message(FATAL_ERROR "setup_third_party: NAME is required")
    endif()
    if(NOT ARG_GIT_REPOSITORY)
        message(FATAL_ERROR "setup_third_party: GIT_REPOSITORY is required")
    endif()
    if(NOT ARG_GIT_TAG)
        message(FATAL_ERROR "setup_third_party: GIT_TAG is required")
    endif()

    # Set third_party directory at project root
    set(THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/third_party")
    set(THIRD_PARTY_SRC_DIR "${THIRD_PARTY_DIR}/${ARG_NAME}")

    # Check if third-party source already exists and is valid
    if(EXISTS "${THIRD_PARTY_SRC_DIR}")
        # Check if it contains CMakeLists.txt (indicating valid source)
        if(EXISTS "${THIRD_PARTY_SRC_DIR}/CMakeLists.txt")
            message(STATUS "Already Download And Skip Download: ${ARG_NAME}")

            # Add the third-party subdirectory directly
            if(ARG_EXCLUDE_FROM_ALL)
                add_subdirectory("${THIRD_PARTY_SRC_DIR}" "${CMAKE_BINARY_DIR}/third_party/${ARG_NAME}" EXCLUDE_FROM_ALL)
            else()
                add_subdirectory("${THIRD_PARTY_SRC_DIR}" "${CMAKE_BINARY_DIR}/third_party/${ARG_NAME}")
            endif()
            return()
        else()
            message(WARNING "third_party/${ARG_NAME} exists but appears invalid (missing CMakeLists.txt)")
        endif()
    endif()

    # Source not found or invalid, download using FetchContent
    message(STATUS "No Third Party Source Found, Download ${ARG_NAME}")

    # Declare the content to fetch
    FetchContent_Declare(
        ${ARG_NAME}
        GIT_REPOSITORY ${ARG_GIT_REPOSITORY}
        GIT_TAG        ${ARG_GIT_TAG}
        GIT_SHALLOW    TRUE
    )

    # Set options for excluding from all if requested
    if(ARG_EXCLUDE_FROM_ALL)
        set(FETCHCONTENT_SOURCE_DIR_${ARG_NAME} "${THIRD_PARTY_SRC_DIR}")
        # Note: FetchContent doesn't directly support EXCLUDE_FROM_ALL,
        # but we can handle it after makeAvailable if needed
    endif()

    # Try to make the content available
    FetchContent_MakeAvailable(${ARG_NAME})

    # Optional: Copy downloaded source to third_party for future use
    # Only do this if download was successful and third_party dir doesn't exist
    if(NOT EXISTS "${THIRD_PARTY_SRC_DIR}" AND EXISTS "${${ARG_NAME}_SOURCE_DIR}")
        message(STATUS "Copying ${ARG_NAME} to third_party directory for future use...")
        file(COPY "${${ARG_NAME}_SOURCE_DIR}/"
             DESTINATION "${THIRD_PARTY_SRC_DIR}")
    endif()
endfunction()

# =============================================================================
# Macro: setup_third_party_quiet
# =============================================================================
# Same as setup_third_party but suppresses download progress messages
# =============================================================================
macro(setup_third_party_quiet)
    set(FETCHCONTENT_QUIET TRUE)
    setup_third_party(${ARGN})
    set(FETCHCONTENT_QUIET FALSE)
endmacro()
