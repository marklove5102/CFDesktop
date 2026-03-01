log_info("TextArea Example" "Building Material TextArea Example")

add_executable(textarea_example
    main.cpp
    TextAreaExampleWindow.h
    TextAreaExampleWindow.cpp
)

target_link_libraries(textarea_example PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    cfui
)

if(WIN32)
    set_target_properties(textarea_example PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    cf_register_qt_deployment(textarea_example)
endif()
