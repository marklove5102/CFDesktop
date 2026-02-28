log_info("UI_Components" "Start Configure the UI Components")

# Include subdirectory CMakeLists (STATIC libraries)
add_subdirectory(material)

# ============================================================
# Unified UI Components Library - STATIC library for cfui.dll
# ============================================================

# Library for animation interfaces
add_library(cf_ui_components STATIC
    animation.cpp
    animation_factory_manager.cpp
    timing_animation.cpp
)

# Add include directories
target_include_directories(cf_ui_components PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

# Link against material sub-module
target_link_libraries(cf_ui_components PUBLIC
    cf_ui_components_material
)

log_info("UI_Components" "the UI Components Configuration Done")
