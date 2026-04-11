# ============================================================
# CFDesktop Desktop Settings Generation
# ============================================================
# This module generates:
#   1. desktop_settings.h - compile-time header with default paths
#   2. settings/early.json - pre-configured settings for overlay
#
# Output directories:
#   ${CMAKE_BINARY_DIR}/autogen/desktop_settings/  (header)
#   ${CMAKE_BINARY_DIR}/bin/settings/              (early.json)
#
# Cross-compilation:
#   Set -DCFDESKTOP_DEFAULT_ROOT=<path> to override the default
#   desktop root for the target platform. Without this override,
#   host environment variables are used (which may not be correct
#   when cross-compiling).
#
# Usage:
#   1. Include this file in CMakeLists.txt (after project())
#   2. Call cf_generate_desktop_settings() to generate
#   3. Use CFDESKTOP_SETTINGS_OUTPUT_DIR in target_include_directories
# ============================================================

function(cf_generate_desktop_settings)
    # ----------------------------------------------------------
    # Determine default desktop root
    # Priority: user override > cross-compile defaults > native defaults
    # ----------------------------------------------------------
    if(DEFINED CFDESKTOP_DEFAULT_ROOT)
        # User explicitly provided via -DCFDESKTOP_DEFAULT_ROOT=...
        # Do nothing, use the provided value
        log_info("DesktopSettings" "Using user-specified desktop root: ${CFDESKTOP_DEFAULT_ROOT}")
    elseif(CMAKE_CROSSCOMPILING)
        # Cross-compiling: host env vars are meaningless for the target
        # Use sensible target defaults that don't depend on host environment
        if(WIN32)
            set(CFDESKTOP_DEFAULT_ROOT "C:/Users/cfdesktop/")
        else()
            set(CFDESKTOP_DEFAULT_ROOT "/home/cfdesktop")
        endif()
        log_info("DesktopSettings" "Cross-compilation mode, using target default: ${CFDESKTOP_DEFAULT_ROOT}")
    else()
        # Native build: use host environment variables
        if(WIN32)
            # Normalize to forward slashes to avoid Qt6 QSettings treating
            # backslashes as escape sequences in the INI file
            file(TO_CMAKE_PATH "$ENV{USERPROFILE}/cfdesktop/" CFDESKTOP_DEFAULT_ROOT)
        else()
            set(CFDESKTOP_DEFAULT_ROOT "$ENV{HOME}/desktop")
        endif()
    endif()

    # ----------------------------------------------------------
    # 1. Generate desktop_settings.h (compile-time header)
    # ----------------------------------------------------------
    set(CFDESKTOP_SETTINGS_OUTPUT_DIR ${CMAKE_BINARY_DIR}/autogen/desktop_settings)
    file(MAKE_DIRECTORY ${CFDESKTOP_SETTINGS_OUTPUT_DIR})

    configure_file(
        ${CMAKE_SOURCE_DIR}/cmake/meta_info/desktop_settings.template.h.in
        ${CFDESKTOP_SETTINGS_OUTPUT_DIR}/desktop_settings.h
        @ONLY
    )

    # Export output directory to parent scope for use in subdirectories
    set(CFDESKTOP_SETTINGS_OUTPUT_DIR ${CFDESKTOP_SETTINGS_OUTPUT_DIR} PARENT_SCOPE)

    log_info("DesktopSettings" "Generated desktop_settings.h in: ${CFDESKTOP_SETTINGS_OUTPUT_DIR}")
    log_info("DesktopSettings" "Desktop root default: ${CFDESKTOP_DEFAULT_ROOT}")

    # ----------------------------------------------------------
    # 2. Generate settings/early.json (pre-configured for overlay)
    # ----------------------------------------------------------
    set(EARLY_JSON_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/settings)
    file(MAKE_DIRECTORY ${EARLY_JSON_OUTPUT_DIR})

    # Only generate if not already present (preserve developer customizations)
    set(EARLY_JSON_PATH ${EARLY_JSON_OUTPUT_DIR}/early.json)

    if(NOT EXISTS "${EARLY_JSON_PATH}" OR CFDESKTOP_OVERLAY_FORCE)
        configure_file(
            ${CMAKE_SOURCE_DIR}/cmake/meta_info/early_settings.template.json.in
            ${EARLY_JSON_PATH}
            @ONLY
        )
        log_info("DesktopSettings" "Generated settings/early.json in: ${EARLY_JSON_OUTPUT_DIR}")
    else()
        log_info("DesktopSettings" "settings/early.json already exists, skipping generation")
    endif()

    # ----------------------------------------------------------
    # 3. Export cross-compilation status for overlay and other modules
    # ----------------------------------------------------------
    # ----------------------------------------------------------
    # 3. Export for overlay and other modules
    # ----------------------------------------------------------
    set(CFDESKTOP_DEFAULT_ROOT ${CFDESKTOP_DEFAULT_ROOT} PARENT_SCOPE)
    set(CFDESKTOP_CROSSCOMPILING ${CMAKE_CROSSCOMPILING} PARENT_SCOPE)
    set(CFDESKTOP_TARGET_PLATFORM ${CMAKE_SYSTEM_NAME} PARENT_SCOPE)
endfunction()
