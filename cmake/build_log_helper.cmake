# Build Log Helper
# Provides convenient logging functions for CMake builds

# Print a section banner
function(print_banner banner_text)
    set(BANNER_WIDTH 60)
    string(LENGTH "${banner_text}" TEXT_LENGTH)

    # Calculate padding, ensure it's not negative
    math(EXPR PADDING "${BANNER_WIDTH} - ${TEXT_LENGTH} - 2")
    if(PADDING LESS 0)
        set(PADDING 0)
    endif()

    # Calculate left and right padding
    math(EXPR HALF_PAD "${PADDING} / 2")
    math(EXPR RIGHT_PAD "${PADDING} - ${HALF_PAD}")

    string(REPEAT "=" "${HALF_PAD}" LEFT_EQ)
    string(REPEAT "=" "${RIGHT_PAD}" RIGHT_EQ)

    message("")
    message("${LEFT_EQ} ${banner_text} ${RIGHT_EQ}")
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

# Print build configuration summary
function(log_config_summary)
    print_banner("Build Configuration")

    string(TIMESTAMP CURRENT_TIMESTAMP "%Y-%m-%d %H:%M:%S")
    message("Build Time: ${CURRENT_TIMESTAMP}")
    message("Project: ${PROJECT_NAME}")
    message("Version: ${PROJECT_VERSION}")
    message("CMake Version: ${CMAKE_VERSION}")
    message("Build Type: ${CMAKE_BUILD_TYPE}")
    message("C++ Flags: ${CMAKE_CXX_FLAGS}")

    # Use variable indirection for nested expansion
    set(_cxx_flags_var "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}")
    set(_link_flags_var "CMAKE_EXE_LINKER_FLAGS_${CMAKE_BUILD_TYPE}")
    message("C++ Flags (${CMAKE_BUILD_TYPE}): ${${_cxx_flags_var}}")
    message("Linker Flags: ${CMAKE_EXE_LINKER_FLAGS}")
    message("Linker Flags (${CMAKE_BUILD_TYPE}): ${${_link_flags_var}}")

    if(CMAKE_CXX_COMPILER)
        message("CXX Compiler: ${CMAKE_CXX_COMPILER}")
    endif()

    if(CMAKE_CXX_STANDARD)
        message("CXX Standard: ${CMAKE_CXX_STANDARD}")
    endif()

    message("")
endfunction()

# Using After the find packages
function(log_qt_dir)
    # 打印 Qt 找到的路径
    message(STATUS "Qt6 Dir: ${Qt6_DIR}")
    message(STATUS "Qt6 Install Prefix: ${Qt6Core_DIR}")

    # 打印各组件的实际库文件路径
    foreach(_qt_lib Widgets Gui Core)
        get_target_property(_loc Qt6::${_qt_lib} IMPORTED_LOCATION_RELEASE)
        get_target_property(_loc_dbg Qt6::${_qt_lib} IMPORTED_LOCATION_DEBUG)
        message(STATUS "Qt6::${_qt_lib} Release => ${_loc}")
        message(STATUS "Qt6::${_qt_lib} Debug   => ${_loc_dbg}")
    endforeach()
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
