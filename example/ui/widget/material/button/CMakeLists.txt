log_info("Button Example" "Building Material Button Example")

add_executable(button_example
    main.cpp
    ButtonExampleWindow.h
    ButtonExampleWindow.cpp
)

target_link_libraries(button_example PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    cfui
)

if(WIN32)
    set_target_properties(button_example PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    cf_register_qt_deployment(button_example)
endif()
