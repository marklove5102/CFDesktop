log_info("CheckBox Example" "Building Material CheckBox Example")

add_executable(checkbox_example
    main.cpp
    CheckBoxExampleWindow.h
    CheckBoxExampleWindow.cpp
)

target_link_libraries(checkbox_example PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    cfui
)

if(WIN32)
    set_target_properties(checkbox_example PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    cf_register_qt_deployment(checkbox_example)
endif()
