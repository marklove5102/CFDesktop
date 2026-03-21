add_library(cfbase_network STATIC
    network.cpp
)

target_link_libraries(cfbase_network
    PUBLIC  Qt6::Core       
    PRIVATE Qt6::Network
)

target_include_directories(cfbase_network     
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../..)