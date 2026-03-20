log_info("UI" "Start UI Components Configurations")

# Base Supports the UI Components Needs
add_subdirectory(base)

# Core Provides the UI Components Supports
add_subdirectory(core)

# Components Provides animation and UI component implementations
add_subdirectory(components)

# Widget Supports the Middle Level Widgets Abstractions
add_subdirectory(widget)

# Models will provide SDK Like Models for programmers
# Quick Start Apps
add_subdirectory(models)

# ============================================================
# Unified UI Library - Static Library
# ============================================================

add_library(cfui STATIC)

# Link against STATIC libraries from sub-modules
# Order: dependent libraries first, then their dependencies
target_link_libraries(cfui PUBLIC
    cf_ui_core
    cf_ui_base
    cf_ui_components
    cf_ui_widget
)

# Set include directories
target_include_directories(cfui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Link Qt libraries and base library (SHARED cfbase.dll)
target_link_libraries(cfui PUBLIC
    Qt6::Core
    Qt6::Gui
    CFDesktop::base
)

# Add alias for consistent naming
add_library(UI::cfui ALIAS cfui)
# For backward compatibility
add_library(UI::cf_ui ALIAS cfui)
add_library(UI::base ALIAS cfui)

log_info("UI" "Start UI Components Configurations Done")
