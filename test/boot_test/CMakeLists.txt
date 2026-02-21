# 控制台版本可执行文件
add_executable(boot_test_core
    boot_test_core.cpp
    boot_detect.cpp
)

target_link_libraries(boot_test_core
    PRIVATE
    Qt6::Core
)

add_executable(boot_test_gui
    boot_test_gui.cpp
    boot_detect.cpp
)

target_link_libraries(boot_test_gui
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Windows 下设置为 GUI 应用（不显示控制台窗口）
if(WIN32)
    set_target_properties(boot_test_gui PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# 安装目标
install(TARGETS boot_test_core boot_test_gui
    RUNTIME DESTINATION bin
)

qt_add_windows_deploy(boot_test_core AUTO_DEPLOY INSTALL_DEPLOY)
qt_add_windows_deploy(boot_test_gui  AUTO_DEPLOY INSTALL_DEPLOY)
