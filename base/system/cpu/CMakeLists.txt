# CPU Module - compiled as OBJECT library for merging into cfbase.dll
cmake_minimum_required(VERSION 3.16)

add_library(cfbase_cpu STATIC)
set_target_properties(cfbase_cpu PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
# Define export macro since this will be linked into cfbase shared library
target_compile_definitions(cfbase_cpu PUBLIC CFBASE_EXPORTS)
target_include_directories(cfbase_cpu
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
)
target_sources(cfbase_cpu PRIVATE
    cfcpu.cpp
    cfcpu_profile.cpp
    cfcpu_bonus.cpp
)

if(WIN32)
    target_sources(cfbase_cpu PRIVATE
        private/win_impl/cpu_profile.cpp
        private/win_impl/cpu_bonus.cpp
        private/win_impl/cpu_features.cpp
        private/win_impl/cpu_info.cpp
    )
    target_include_directories(cfbase_cpu PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private
    )
elseif(UNIX)
    target_sources(cfbase_cpu PRIVATE
        private/linux_impl/cpu_profile.cpp
        private/linux_impl/cpu_bonus.cpp
        private/linux_impl/cpu_features.cpp
        private/linux_impl/cpu_info.cpp
        ../../utils/linux/proc_parser.cpp
    )
endif()
