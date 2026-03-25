# Material Motion Spec Gallery Example
log_info("Material Motion Spec" "Building Motion Spec Gallery Example")

add_executable(material_motion_spec_gallery
    main.cpp
    MaterialMotionSpecMainWindow.h
    MaterialMotionSpecMainWindow.cpp
)

target_link_libraries(material_motion_spec_gallery PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    cfui
)

# Set output directory to examples/gui/
cf_set_example_output_dir(material_motion_spec_gallery "gui")
cf_register_example_launcher(material_motion_spec_gallery "gui")

# Set as GUI application on Windows (hide console window)
if(WIN32)
    set_target_properties(material_motion_spec_gallery PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
