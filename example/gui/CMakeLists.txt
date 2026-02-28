log_info("Example GUI" "Managing GUI Examples")

# 包含统一部署工具（在子项目之前）
if(WIN32)
    include(${CMAKE_SOURCE_DIR}/cmake/QtDeployUtils.cmake)
endif()

add_subdirectory(material)
add_subdirectory(theme)

# ============================================================
# Unified Qt Deployment (Windows only)
# ============================================================
# 在所有子项目之后调用，统一部署已注册的可执行文件
if(WIN32)
    cf_setup_unified_qt_deployment()
endif()
