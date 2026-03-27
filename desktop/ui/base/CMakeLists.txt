# Desktop UI Widgets
# Convenient widget components for desktop-specific UI

# Create interface library for convenience
add_library(cf_desktop_ui_base STATIC)

target_sources(cf_desktop_ui_base PRIVATE
    qt_backend.cpp
    meta_adapts.cpp
)

target_include_directories(cf_desktop_ui_base PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
)

target_link_libraries(cf_desktop_ui_base 
PUBLIC
    cfbase
PRIVATE
    Qt6::Core Qt6::Gui Qt6::Widgets
)
