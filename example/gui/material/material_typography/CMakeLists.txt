# Material Typography Gallery Example
log_info("Material Typography" "Building Typography Gallery Example")

add_executable(material_typography_gallery
    main.cpp
    MaterialTypographyMainWindow.h
    MaterialTypographyMainWindow.cpp
)

target_link_libraries(material_typography_gallery PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    cfui
)

# Set output directory to examples/gui/
cf_set_example_output_dir(material_typography_gallery "gui")
cf_register_example_launcher(material_typography_gallery "gui")

# Set as GUI application on Windows (hide console window)
if(WIN32)
    set_target_properties(material_typography_gallery PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
