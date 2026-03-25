log_info("Desktop Init Module" "Start Configuring Desktop Init")

# Note: gui_progress subdirectory removed - UI components migrated to desktop/ui/widget/init_session/
add_subdirectory(early_gain)

add_library(CFDesktopInit STATIC)

target_sources(CFDesktopInit PRIVATE
    init_early_session.cpp
    init_session_chain.cpp
    init_session.cpp
    init_settings.cpp
    gui_progress/gui_init_stage.cpp
    ${EARLY_GAIN_SOURCES}
)

target_include_directories(CFDesktopInit
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(CFDesktopInit PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    CFDesktopEarlySession
    cf_desktop_ui_widget_init_session  # Link to UI widget library (formerly gui_progress)
)

log_info("Desktop Init Module" "Start Configuring Desktop Finish")