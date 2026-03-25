# Making Components with early sessions

add_library(CFDesktopPathSettings STATIC)

target_sources(CFDesktopPathSettings PRIVATE 
    desktop_main_path_resolvers.cpp
    desktop_self_path_resolvers.cpp)

target_include_directories(CFDesktopPathSettings PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(CFDesktopPathSettings PUBLIC cflogger Qt6::Core)
