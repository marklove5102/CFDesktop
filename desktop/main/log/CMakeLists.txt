# Making Components with early sessions

add_library(CFDesktopLog STATIC)

target_sources(CFDesktopLog PRIVATE log_helper.cpp)

target_include_directories(CFDesktopLog PUBLIC .)

target_link_libraries(CFDesktopLog PUBLIC cflogger Qt6::Core)
