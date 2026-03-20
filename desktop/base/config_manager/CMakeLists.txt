# Config manager module
cmake_minimum_required(VERSION 3.16)

# Static library (has .cpp sources)
add_library(cfconfig STATIC)

# Sources
target_sources(cfconfig PRIVATE
    src/cfconfig.cpp
    src/cfconfig_path_provider.cpp
    src/impl/config_impl.cpp
    src/impl/config_key.cpp
)

# Include directories
target_include_directories(cfconfig PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link dependencies
target_link_libraries(cfconfig PUBLIC
    CFDesktop::base
    Qt6::Core
)

# Alias for consistent naming
add_library(CFDesktop::config ALIAS cfconfig)
