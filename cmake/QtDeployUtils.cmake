# ============================================================
# Qt Unified Deployment Utilities
# ============================================================
# 提供动态注册机制，让子项目自动注册到统一部署列表

# 定义一个全局列表变量，用于收集所有需要部署的可执行文件
set(QT_DEPLOY_EXECUTABLES "" CACHE INTERNAL "List of executables for Qt deployment")

# ============================================================
# 宏: cf_register_qt_deployment
# ============================================================
# 子项目调用此宏注册自己到统一部署列表
#
# 用法:
#   cf_register_qt_deployment(target_name)
#
macro(cf_register_qt_deployment TARGET_NAME)
    list(APPEND QT_DEPLOY_EXECUTABLES ${TARGET_NAME})
    set(QT_DEPLOY_EXECUTABLES "${QT_DEPLOY_EXECUTABLES}" CACHE INTERNAL "List of executables for Qt deployment")
endmacro()

# ============================================================
# 函数: cf_setup_unified_qt_deployment
# ============================================================
# 在顶层调用，设置统一的 Qt 部署
# 必须在所有子项目 add_subdirectory 之后调用
#
function(cf_setup_unified_qt_deployment)
    if(WIN32 AND NOT CMAKE_CROSSCOMPILING AND QT_DEPLOY_EXECUTABLES)
        # 找到 windeployqt
        find_program(WINDEPLOYQT_EXECUTABLE windeployqt
            HINTS "${CMAKE_PREFIX_DIR}/bin" "${_qt_install_prefix}/bin"
        )

        if(WINDEPLOYQT_EXECUTABLE)
            set(DEPLOY_BIN_DIR "${CMAKE_BINARY_DIR}/bin")
            set(DEPLOY_MARKER "${DEPLOY_BIN_DIR}/.deploy_done")

            message(STATUS "Setting up unified Qt deployment for: ${QT_DEPLOY_EXECUTABLES}")

            # 创建统一的部署 target
            add_custom_target(deploy_qt_dlls ALL)

            # 让部署 target 依赖所有已注册的可执行文件
            foreach(exe ${QT_DEPLOY_EXECUTABLES})
                if(TARGET ${exe})
                    add_dependencies(deploy_qt_dlls ${exe})
                    # 确保输出到 bin 目录
                    set_target_properties(${exe} PROPERTIES
                        RUNTIME_OUTPUT_DIRECTORY ${DEPLOY_BIN_DIR}
                    )
                else()
                    message(WARNING "Registered deployment target '${exe}' not found")
                endif()
            endforeach()

            # 使用第一个可执行文件作为 windeployqt 的入口
            list(GET QT_DEPLOY_EXECUTABLES 0 FIRST_EXE)

            # 执行部署
            add_custom_command(TARGET deploy_qt_dlls POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E env QT_HASH_SEED=0
                "${WINDEPLOYQT_EXECUTABLE}"
                    --dir "${DEPLOY_BIN_DIR}"
                    --no-compiler-runtime
                    --no-translations
                    --no-system-d3d-compiler
                    --no-opengl-sw
                    --verbose 1
                "$<TARGET_FILE:${FIRST_EXE}>"
                COMMAND ${CMAKE_COMMAND} -E touch "${DEPLOY_MARKER}"
                COMMENT "Unified Qt deployment to ${DEPLOY_BIN_DIR}"
                VERBATIM
            )
        else()
            message(WARNING "windeployqt not found, skipping automatic Qt deployment")
        endif()
    endif()
endfunction()
