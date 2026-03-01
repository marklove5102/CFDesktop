log_info("TextField Example" "Building Material TextField Example")

add_executable(textfield_example
    main.cpp
    TextFieldExampleWindow.h
    TextFieldExampleWindow.cpp
)

target_link_libraries(textfield_example PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    cfui
)

if(WIN32)
    set_target_properties(textfield_example PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    cf_register_qt_deployment(textfield_example)
endif()
