# Build Log Helper
# Provides convenient logging functions for CMake builds

# ANSI color codes for CMake
set(ESC "")
set(COLOR_RESET "${ESC}[0m")
set(COLOR_CYAN "${ESC}[36m")
set(COLOR_YELLOW "${ESC}[33m")
set(COLOR_GREEN "${ESC}[32m")
set(COLOR_RED "${ESC}[31m")

# Print a section banner with the new style
function(print_banner banner_text)
    set(BORDER_WIDTH 54)
    string(REPEAT "=" "${BORDER_WIDTH}" TOP_BORDER)

    string(LENGTH "${banner_text}" TEXT_LENGTH)
    math(EXPR PADDING "${BORDER_WIDTH} - ${TEXT_LENGTH}")
    if(PADDING LESS 0)
        set(PADDING 0)
    endif()
    math(EXPR HALF_PAD "${PADDING} / 2")
    string(REPEAT " " "${HALF_PAD}" LEFT_PAD)

    message("")
    message("${TOP_BORDER}")
    message("${LEFT_PAD}${banner_text}")
    message("${TOP_BORDER}")
    message("")
endfunction()

# Print an informational message with timestamp
function(log_info module msg)
    string(TIMESTAMP CURRENT_TIMESTAMP "%Y-%m-%d %H:%M:%S")
    message("[${CURRENT_TIMESTAMP}] [INFO] [${module}] ${msg}")
endfunction()

# Print a warning message with timestamp
function(log_warn module msg)
    string(TIMESTAMP CURRENT_TIMESTAMP "%Y-%m-%d %H:%M:%S")
    message("[${CURRENT_TIMESTAMP}] [WARN] [${module}] ${msg}")
endfunction()

# Print an error message with timestamp
function(log_error module msg)
    string(TIMESTAMP CURRENT_TIMESTAMP "%Y-%m-%d %H:%M:%S")
    message(FATAL_ERROR "[${CURRENT_TIMESTAMP}] [ERROR] [${module}] ${msg}")
endfunction()

# Helper function to format a key-value row
# Format:   key (right-aligned) : value
function(_format_kv_row key value max_key_width result)
    string(LENGTH "${key}" key_len)
    if(key_len LESS ${max_key_width})
        math(EXPR padding "${max_key_width} - ${key_len}")
        string(REPEAT " " "${padding}" pad_str)
        set(${result} "  ${key}${pad_str} : ${value}" PARENT_SCOPE)
    else()
        set(${result} "  ${key} : ${value}" PARENT_SCOPE)
    endif()
endfunction()

# Print a section header with arrow prefix
function(_print_section_header text)
    message("")
    message("▶ ${text}")
endfunction()

# Print build configuration summary in a clean format
function(log_build_summary_table)
    set(BORDER_WIDTH 54)
    string(REPEAT "=" "${BORDER_WIDTH}" TOP_BORDER)

    # Get uppercase build type for flag variable names
    string(TOUPPER "${CMAKE_BUILD_TYPE}" _build_type_upper)
    set(_cxx_flags_var "CMAKE_CXX_FLAGS_${_build_type_upper}")

    # Get C++ standard (try multiple sources)
    set(_cxx_standard "unknown")
    if(CMAKE_CXX_STANDARD)
        set(_cxx_standard "${CMAKE_CXX_STANDARD}")
    elseif(CMAKE_CXX_STANDARD_REQUIRED)
        set(_cxx_standard "${CMAKE_CXX_STANDARD_REQUIRED}")
    endif()

    # Get Qt version
    set(QT_VERSION "Not found")
    set(QT_PATH "(not set)")
    if(Qt6_FOUND)
        if(TARGET Qt6::Core)
            get_target_property(QT_VERSION Qt6::Core VERSION)
        endif()
        if(NOT QT_VERSION OR QT_VERSION STREQUAL "NOTFOUND")
            if(DEFINED Qt6Core_VERSION)
                set(QT_VERSION "${Qt6Core_VERSION}")
            elseif(DEFINED Qt6_VERSION)
                set(QT_VERSION "${Qt6_VERSION}")
            else()
                set(QT_VERSION "found")
            endif()
        endif()
        if(Qt6_DIR)
            set(QT_PATH "${Qt6_DIR}")
        endif()
    endif()

    # Get flags with fallback
    set(_cxx_flags "${CMAKE_CXX_FLAGS}")
    if(_cxx_flags STREQUAL "")
        set(_cxx_flags "(empty)")
    endif()

    set(_cxx_type_flags "${${_cxx_flags_var}}")
    if(_cxx_type_flags STREQUAL "")
        set(_cxx_type_flags "(empty)")
    endif()

    set(_link_flags "${CMAKE_EXE_LINKER_FLAGS}")
    if(_link_flags STREQUAL "")
        set(_link_flags "(empty)")
    endif()

    string(TIMESTAMP CURRENT_TIMESTAMP "%Y-%m-%d %H:%M:%S")

    # Print header
    message("")
    message("${TOP_BORDER}")
    message("        ${PROJECT_NAME} Build Configuration")
    message("${TOP_BORDER}")

    # General Section
    _print_section_header("General")
    _format_kv_row("Project Name" "${PROJECT_NAME}" 18 ROW)
    message("${ROW}")
    _format_kv_row("Version" "${PROJECT_VERSION}" 18 ROW)
    message("${ROW}")
    _format_kv_row("Build Time" "${CURRENT_TIMESTAMP}" 18 ROW)
    message("${ROW}")
    _format_kv_row("Build Type" "${CMAKE_BUILD_TYPE}" 18 ROW)
    message("${ROW}")
    _format_kv_row("CMake Version" "${CMAKE_VERSION}" 18 ROW)
    message("${ROW}")

    # Compiler & Flags Section
    _print_section_header("Compiler & Flags")
    if(CMAKE_CXX_COMPILER)
        _format_kv_row("C++ Compiler" "${CMAKE_CXX_COMPILER}" 18 ROW)
    else()
        _format_kv_row("C++ Compiler" "(not set)" 18 ROW)
    endif()
    message("${ROW}")
    _format_kv_row("C++ Standard" "${_cxx_standard}" 18 ROW)
    message("${ROW}")
    _format_kv_row("C++ Flags" "${_cxx_flags}" 18 ROW)
    message("${ROW}")
    _format_kv_row("Debug Flags" "${_cxx_type_flags}" 18 ROW)
    message("${ROW}")
    _format_kv_row("Linker Flags" "${_link_flags}" 18 ROW)
    message("${ROW}")

    # Qt Configuration Section
    _print_section_header("Qt Configuration")
    _format_kv_row("Qt Version" "${QT_VERSION}" 18 ROW)
    message("${ROW}")
    _format_kv_row("Qt Path" "${QT_PATH}" 18 ROW)
    message("${ROW}")
    _format_kv_row("Components" "Core, Gui, Widgets, Network" 18 ROW)
    message("${ROW}")
    _format_kv_row("AUTOMOC/AUTORCC/AUTOUIC" "${CMAKE_AUTOMOC} / ${CMAKE_AUTORCC} / ${CMAKE_AUTOUIC}" 18 ROW)
    message("${ROW}")

    # Early Settings Section
    _print_section_header("Early Settings")
    _format_kv_row("Settings Path" "settings/early.ini" 18 ROW)
    message("${ROW}")
    if(DEFINED CFDESKTOP_DEFAULT_ROOT)
        _format_kv_row("Desktop Root" "${CFDESKTOP_DEFAULT_ROOT}" 18 ROW)
    elseif(WIN32)
        _format_kv_row("Desktop Root" "$ENV{USERPROFILE}/cfdesktop/" 18 ROW)
    else()
        _format_kv_row("Desktop Root" "$ENV{HOME}/desktop" 18 ROW)
    endif()
    message("${ROW}")
    _format_kv_row("Template" "desktop_settings.template.h.in" 18 ROW)
    message("${ROW}")
    _format_kv_row("Auto-generate INI" "early_settings.template.ini.in" 18 ROW)
    message("${ROW}")
    if(CMAKE_CROSSCOMPILING)
        _format_kv_row("Cross-Compile" "YES (${CMAKE_SYSTEM_NAME})" 18 ROW)
    else()
        _format_kv_row("Cross-Compile" "NO (native)" 18 ROW)
    endif()
    message("${ROW}")

    # Desktop Overlay Section
    _print_section_header("Desktop Overlay")
    set(_overlay_dir "${CMAKE_SOURCE_DIR}/cmake/desktop_overlay")
    if(IS_DIRECTORY "${_overlay_dir}")
        file(GLOB_RECURSE _overlay_files "${_overlay_dir}/*")
        list(LENGTH _overlay_files _overlay_count)
        # Filter out .gitkeep
        set(_real_count 0)
        foreach(_f ${_overlay_files})
            get_filename_component(_fname "${_f}" NAME)
            if(NOT _fname STREQUAL ".gitkeep")
                math(EXPR _real_count "${_real_count} + 1")
            endif()
        endforeach()
        if(_real_count GREATER 0)
            _format_kv_row("Overlay Files" "${_real_count} file(s)" 18 ROW)
        else()
            _format_kv_row("Overlay Files" "(empty)" 18 ROW)
        endif()
    else()
        _format_kv_row("Overlay Files" "(no overlay dir)" 18 ROW)
    endif()
    message("${ROW}")
    if(CMAKE_CROSSCOMPILING OR WIN32)
        if(WIN32)
            _format_kv_row("Platform Overlay" "windows/" 18 ROW)
        else()
            _format_kv_row("Platform Overlay" "${CMAKE_SYSTEM_NAME}/" 18 ROW)
        endif()
        message("${ROW}")
    endif()

    # Modules Section
    _print_section_header("Modules")
    message("  base              : ON")
    message("  ui                : ON")
    message("  desktop           : ON")
    message("  example           : ON")
    message("  test              : ON")

    # Print footer
    message("")
    message("${TOP_BORDER}")
    message("")
endfunction()


# Log module start
function(log_module_start module_name)
    print_banner("Building: ${module_name}")
    log_info("CMake" "Starting to build module: ${module_name}")
endfunction()

# Log module end
function(log_module_end module_name)
    log_info("CMake" "Successfully completed module: ${module_name}")
    message("")
endfunction()
