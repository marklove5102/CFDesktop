# ASCII Art module
cmake_minimum_required(VERSION 3.16)

# Static library
add_library(cfasciiart STATIC)

# Sources
target_sources(cfasciiart PRIVATE
    src/cfasciiart.cpp
)

# Include directories
target_include_directories(cfasciiart PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link dependencies
target_link_libraries(cfasciiart PUBLIC
    CFDesktop::base
)

# Alias for consistent naming
add_library(CFDesktop::ascii_art ALIAS cfasciiart)
