
# Recursively Adds
add_subdirectory(widget)
add_subdirectory(models)

add_library(CFDesktopUi STATIC)

target_sources(CFDesktopUi PRIVATE
    CFDesktop.cpp
    CFDesktopProxy.cpp
)

target_link_libraries(CFDesktopUi
PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    cfbase
    cflogger
    cf_desktop_ui_widget  # Link widget submodule
)

