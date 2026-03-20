# Making Components with early sessions

add_library(CFDesktopEarlySession STATIC)

target_sources(CFDesktopEarlySession PRIVATE 
    init_chain/init_chain.cpp
    settings/early_settings.cpp
    impl/early_welcome_impl.cpp
    impl/logger_stage.cpp
    impl/early_config_stage.cpp)

target_include_directories(CFDesktopEarlySession PUBLIC . ..)

# Link against cfbase to access device headers and other base utilities
# Link against cflogger to access cflog.h
target_link_libraries(CFDesktopEarlySession PUBLIC
    cfbase cflogger cfasciiart cffilesystem
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets)






