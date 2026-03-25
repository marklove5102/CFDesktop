# Material Color Scheme Gallery Example
log_info("Material Color Scheme" "Building Color Gallery Example")

add_executable(material_color_scheme_gallery
    main.cpp
    MaterialColorSchemeMainWindow.h
    MaterialColorSchemeMainWindow.cpp
)

target_link_libraries(material_color_scheme_gallery PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    cfui
)

# Set output directory to examples/gui/
cf_set_example_output_dir(material_color_scheme_gallery "gui")
cf_register_example_launcher(material_color_scheme_gallery "gui")

# Set as GUI application on Windows (hide console window)
if(WIN32)
    set_target_properties(material_color_scheme_gallery PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
