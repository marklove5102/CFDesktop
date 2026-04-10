# ============================================================
# CFDesktop Desktop Overlay
# ============================================================
# Two-phase overlay deployment:
#
#   Phase A: Runtime overlay (settings/, config/, logger/)
#            -> ${CMAKE_BINARY_DIR}/bin/
#
#   Phase B: Desktop root overlay (Home/, Desktop/, Documents/,
#            Downloads/, Music/, Pictures/, Videos/, Apps/, Runtime/)
#            -> ${CFDESKTOP_DEFAULT_ROOT}
#
# Overlay directory structure (source):
#   cmake/desktop_overlay/             <- Generic overlay (all platforms)
#   cmake/desktop_overlay/windows/     <- Windows-specific overlay
#   cmake/desktop_overlay/linux/       <- Linux-specific overlay
#
# Priority: platform-specific > generic (platform overlay wins)
#
# The runtime's "skip if exists" logic in EarlyConfigStage naturally
# picks up overlay files — no C++ code changes needed.
#
# To force overwrite existing files, set:
#   -DCFDESKTOP_OVERLAY_FORCE=ON
#
# Cross-compilation deployment:
#   cmake --install <build_dir> --prefix <deploy_dir>
# ============================================================

# Runtime-related directories (go to bin/)
set(_CF_RUNTIME_DIRS "settings" "config" "logger")

# Desktop root directories (go to CFDESKTOP_DEFAULT_ROOT)
set(_CF_DESKTOP_ROOT_DIRS "Home" "Desktop" "Documents" "Downloads"
                          "Music" "Pictures" "Videos" "Apps" "Runtime")

# Reserved subdirectory names for platform-specific overlays
set(_CF_PLATFORM_DIRS "windows" "linux" "darwin")


# ============================================================
# Phase A: Runtime overlay -> ${CMAKE_BINARY_DIR}/bin/
# ============================================================
# Copies runtime files (settings/, config/, logger/) to the build
# output directory. Called from check_pre_configure.cmake.
# ============================================================
function(cf_apply_desktop_overlay)
    set(OVERLAY_BASE "${CMAKE_SOURCE_DIR}/cmake/desktop_overlay")

    if(NOT IS_DIRECTORY "${OVERLAY_BASE}")
        log_info("DesktopOverlay" "Overlay directory not found, skipping")
        return()
    endif()

    set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin")
    set(COPIED 0)

    # Apply generic overlay (runtime dirs only)
    _cf_overlay_copy_layer("${OVERLAY_BASE}" "${OUTPUT_DIR}" _CF_RUNTIME_DIRS COPIED)

    # Apply platform-specific overlay (overrides generic)
    _cf_apply_platform_overlay("${OVERLAY_BASE}" "${OUTPUT_DIR}" COPIED)

    if(COPIED GREATER 0)
        log_info("DesktopOverlay" "Applied ${COPIED} runtime overlay file(s)")
    else()
        log_info("DesktopOverlay" "No runtime overlay files to apply")
    endif()
endfunction()


# ============================================================
# Phase B: Desktop root overlay -> ${CFDESKTOP_DEFAULT_ROOT}
# ============================================================
# Copies desktop root content (Home/, Desktop/, etc.) to the
# configured desktop root path. Called AFTER cf_generate_desktop_settings().
# ============================================================
function(cf_apply_desktop_root_overlay)
    set(OVERLAY_BASE "${CMAKE_SOURCE_DIR}/cmake/desktop_overlay")

    if(NOT IS_DIRECTORY "${OVERLAY_BASE}")
        return()
    endif()

    # CFDESKTOP_DEFAULT_ROOT is exported by cf_generate_desktop_settings()
    if(NOT DEFINED CFDESKTOP_DEFAULT_ROOT)
        log_info("DesktopOverlay" "CFDESKTOP_DEFAULT_ROOT not set, skipping desktop root overlay")
        return()
    endif()

    # Skip if default root contains unresolved env vars (cross-compile edge case)
    string(FIND "${CFDESKTOP_DEFAULT_ROOT}" [[$ENV{]] _HAS_ENV_VAR)
    if(NOT _HAS_ENV_VAR EQUAL -1)
        log_info("DesktopOverlay" "Desktop root contains unresolved env vars, skipping: ${CFDESKTOP_DEFAULT_ROOT}")
        return()
    endif()

    set(COPIED 0)

    # Apply generic desktop root overlay
    _cf_overlay_copy_layer("${OVERLAY_BASE}" "${CFDESKTOP_DEFAULT_ROOT}" _CF_DESKTOP_ROOT_DIRS COPIED)

    # Apply platform-specific desktop root overlay
    _cf_apply_platform_overlay("${OVERLAY_BASE}" "${CFDESKTOP_DEFAULT_ROOT}" COPIED)

    if(COPIED GREATER 0)
        log_info("DesktopOverlay" "Applied ${COPIED} desktop root overlay file(s) -> ${CFDESKTOP_DEFAULT_ROOT}")
    else()
        log_info("DesktopOverlay" "No desktop root overlay files to apply")
    endif()
endfunction()


# ============================================================
# Platform-specific overlay dispatch
# ============================================================
function(_cf_apply_platform_overlay OVERLAY_BASE OUTPUT_DIR RESULT_COUNT)
    if(NOT CMAKE_CROSSCOMPILING AND NOT WIN32)
        return()
    endif()

    if(WIN32)
        set(PLATFORM_NAME "windows")
    else()
        string(TOLOWER "${CMAKE_SYSTEM_NAME}" PLATFORM_NAME)
    endif()

    set(PLATFORM_OVERLAY "${OVERLAY_BASE}/${PLATFORM_NAME}")
    if(NOT IS_DIRECTORY "${PLATFORM_OVERLAY}")
        return()
    endif()

    log_info("DesktopOverlay" "Applying platform overlay: ${PLATFORM_NAME}")

    # Platform overlay overwrites generic (FORCE behavior)
    set(_SAVED_FORCE ${CFDESKTOP_OVERLAY_FORCE})
    set(CFDESKTOP_OVERLAY_FORCE ON)
    _cf_overlay_copy_layer("${PLATFORM_OVERLAY}" "${OUTPUT_DIR}" "" COPIED)
    set(CFDESKTOP_OVERLAY_FORCE ${_SAVED_FORCE})

    math(EXPR TOTAL "${${RESULT_COUNT}} + ${COPIED}")
    set(${RESULT_COUNT} ${TOTAL} PARENT_SCOPE)
endfunction()


# ============================================================
# Internal helper: copy one overlay layer
# ============================================================
# LAYER_DIR   - source overlay directory
# OUTPUT_DIR  - destination directory
# FILTER_DIRS - variable name containing dir names to include
#               (empty string = include all)
# RESULT_COUNT - variable to accumulate copied file count
# ============================================================
function(_cf_overlay_copy_layer LAYER_DIR OUTPUT_DIR FILTER_DIRS RESULT_COUNT)
    file(GLOB_RECURSE OVERLAY_FILES RELATIVE "${LAYER_DIR}" "${LAYER_DIR}/*")
    list(LENGTH OVERLAY_FILES FILE_COUNT)

    if(FILE_COUNT EQUAL 0)
        return()
    endif()

    set(_LOCAL_COPIED 0)

    foreach(REL_PATH ${OVERLAY_FILES})
        # Skip placeholder and VCS files
        get_filename_component(FILENAME "${REL_PATH}" NAME)
        if(FILENAME STREQUAL ".gitkeep" OR FILENAME STREQUAL ".gitignore")
            continue()
        endif()

        # Skip platform subdirectories (handled by _cf_apply_platform_overlay)
        get_filename_component(FIRST_SEG "${REL_PATH}" DIRECTORY)
        if(FIRST_SEG)
            string(FIND "${FIRST_SEG}" "/" _SLASH_POS)
            if(_SLASH_POS EQUAL 0)
                string(SUBSTRING "${FIRST_SEG}" 0 ${_SLASH_POS} _TOP_DIR)
            else()
                set(_TOP_DIR "${FIRST_SEG}")
            endif()
            list(FIND _CF_PLATFORM_DIRS "${_TOP_DIR}" _IS_PLATFORM)
            if(NOT _IS_PLATFORM EQUAL -1)
                continue()
            endif()
        endif()

        # Filter by directory scope if specified
        if(FILTER_DIRS)
            # Extract top-level directory name
            get_filename_component(_TOP_LEVEL "${REL_PATH}" DIRECTORY)
            if(_TOP_LEVEL)
                string(REPLACE "/" ";" _PATH_PARTS "${_TOP_LEVEL}")
                list(GET _PATH_PARTS 0 _DIR_NAME)
            else()
                set(_DIR_NAME "${REL_PATH}")
            endif()

            list(FIND ${FILTER_DIRS} "${_DIR_NAME}" _MATCHED)
            if(_MATCHED EQUAL -1)
                continue()
            endif()
        endif()

        set(SRC "${LAYER_DIR}/${REL_PATH}")
        set(DST "${OUTPUT_DIR}/${REL_PATH}")

        # Ensure destination directory exists
        get_filename_component(DST_DIR "${DST}" DIRECTORY)
        file(MAKE_DIRECTORY "${DST_DIR}")

        # Copy file
        if(NOT EXISTS "${DST}" OR CFDESKTOP_OVERLAY_FORCE)
            file(COPY "${SRC}" DESTINATION "${DST_DIR}")
            math(EXPR _LOCAL_COPIED "${_LOCAL_COPIED} + 1")
            log_info("DesktopOverlay" "  -> ${REL_PATH}")
        endif()
    endforeach()

    math(EXPR TOTAL "${${RESULT_COUNT}} + ${_LOCAL_COPIED}")
    set(${RESULT_COUNT} ${TOTAL} PARENT_SCOPE)
endfunction()


# ============================================================
# Install rule: package for cross-compile deployment
# ============================================================
# Usage: cmake --install <build_dir> --prefix <deploy_dir>
# Result: <deploy_dir>/bin/        <- executable + runtime overlay
#         <deploy_dir>/desktop/    <- desktop root overlay
# ============================================================
function(cf_install_desktop_overlay)
    set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin")

    # Install runtime output (binaries + runtime overlay)
    install(DIRECTORY "${OUTPUT_DIR}/"
        DESTINATION bin
        USE_SOURCE_PERMISSIONS
        PATTERN "*.pdb" EXCLUDE
    )

    # Install desktop root overlay (if CFDESKTOP_DEFAULT_ROOT has content)
    if(DEFINED CFDESKTOP_DEFAULT_ROOT AND IS_DIRECTORY "${CFDESKTOP_DEFAULT_ROOT}")
        install(DIRECTORY "${CFDESKTOP_DEFAULT_ROOT}/"
            DESTINATION desktop
            USE_SOURCE_PERMISSIONS
            PATTERN "*.log" EXCLUDE
        )
    endif()

    log_info("DesktopOverlay" "Install rule configured")
endfunction()
