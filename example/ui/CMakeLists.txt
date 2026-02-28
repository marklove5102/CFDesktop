log_info("Example UI" "Managing UI Widget Examples")

# 包含统一部署工具（子项目需要 cf_register_qt_deployment 宏）
# 注意：不调用 cf_setup_unified_qt_deployment()，由 gui 层统一处理
if(WIN32)
    include(${CMAKE_SOURCE_DIR}/cmake/QtDeployUtils.cmake)
endif()

add_subdirectory(widget)
