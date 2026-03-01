log_info("RadioButton Example" "Building Material RadioButton Example")

add_executable(radiobutton_example
    main.cpp
    RadioButtonExampleWindow.h
    RadioButtonExampleWindow.cpp
)

target_link_libraries(radiobutton_example PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    cfui
)

if(WIN32)
    set_target_properties(radiobutton_example PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    cf_register_qt_deployment(radiobutton_example)
endif()
