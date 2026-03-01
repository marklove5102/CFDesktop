log_info("Label Example" "Building Material Label Example")

add_executable(label_example
    main.cpp
    LabelExampleWindow.h
    LabelExampleWindow.cpp
)

target_link_libraries(label_example PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    cfui
)

if(WIN32)
    set_target_properties(label_example PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    cf_register_qt_deployment(label_example)
endif()
