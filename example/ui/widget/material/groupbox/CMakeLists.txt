log_info("GroupBox Example" "Building Material GroupBox Example")

add_executable(groupbox_example
    main.cpp
    GroupBoxExampleWindow.h
    GroupBoxExampleWindow.cpp
)

target_link_libraries(groupbox_example PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    cfui
)

if(WIN32)
    set_target_properties(groupbox_example PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    cf_register_qt_deployment(groupbox_example)
endif()
