log_info("Desktop Main" "Creating unified CFDesktopMain library")

# Collect source variables from subdirectories
add_subdirectory(log)
add_subdirectory(early_session/impl)
add_subdirectory(init/early_gain)

# CFDesktopEarlySession sources (merged from early_session/CMakeLists.txt)
set(EARLY_SESSION_SOURCES
    early_session/init_chain/init_chain.cpp
    early_session/settings/early_settings.cpp
    early_session/early_handle/early_handle.cpp
    ${IMPL_SOURCES}
)

# CFDesktopInit sources (merged from init/CMakeLists.txt)
set(INIT_SOURCES
    init/init_early_session.cpp
    init/init_session_chain.cpp
    init/init_session.cpp
    init/init_settings.cpp
    init/gui_progress/gui_init_stage.cpp
    ${EARLY_GAIN_SOURCES}
)

add_library(CFDesktopMain STATIC)

target_sources(CFDesktopMain PRIVATE
    path/desktop_main_path_resolvers.cpp
    path/desktop_self_path_resolvers.cpp
    ${EARLY_SESSION_SOURCES}
    ${INIT_SOURCES}
)

# Include directories (merged from CFDesktopEarlySession and CFDesktopInit)
target_include_directories(CFDesktopMain PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/early_session>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/init>
)

target_link_libraries(CFDesktopMain PUBLIC
    cfbase cflogger cfasciiart cffilesystem CFDesktopLog
    cf_desktop_ui_widget_init_session
    Qt6::Core Qt6::Gui Qt6::Widgets
)

log_info("Desktop Main" "CFDesktopMain library configured")
