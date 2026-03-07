# =============================================================================
# CI Build Toolchain Configuration (x86_64/AMD64)
# =============================================================================
# 专用工具链文件，用于 Docker 容器内的 x86_64 CI 构建
#
# Qt 安装路径: /opt/Qt/6.8.1/gcc_64
#
# 使用方式：
#   cmake -DUSE_TOOLCHAIN=linux/ci-x86_64 -S . -B build
# =============================================================================

set(CMAKE_SYSTEM_NAME Linux)

# -----------------------------------------------------------------------------
# Qt6 搜索路径（x86_64 容器环境）
# Docker 容器通过 aqtinstall 安装 Qt 到 /opt/Qt/6.8.1/gcc_64
# -----------------------------------------------------------------------------
set(QT6_BASE_DIR "/opt/Qt/6.8.1/gcc_64")
set(QT6_CMAKE_DIR "${QT6_BASE_DIR}/lib/cmake/Qt6")

# 仍允许环境变量覆盖（用于调试或特殊情况）
if(DEFINED ENV{Qt6_DIR})
    set(QT6_CMAKE_DIR "$ENV{Qt6_DIR}")
    get_filename_component(QT6_BASE_DIR "${QT6_CMAKE_DIR}/../.." ABSOLUTE)
elseif(DEFINED ENV{QT6_DIR})
    set(QT6_BASE_DIR "$ENV{QT6_DIR}")
    set(QT6_CMAKE_DIR "${QT6_BASE_DIR}/lib/cmake/Qt6")
endif()

# 设置 Qt6_DIR 和 CMAKE_PREFIX_PATH
set(Qt6_DIR "${QT6_CMAKE_DIR}" CACHE PATH "Qt6 installation directory")
set(CMAKE_PREFIX_PATH "${QT6_BASE_DIR}")
list(APPEND CMAKE_PREFIX_PATH "${QT6_BASE_DIR}")

# -----------------------------------------------------------------------------
# 编译器配置
# 使用系统默认的 GCC，CMake 会自动找到 gcc/g++
# -----------------------------------------------------------------------------

# 启用 ccache 加速构建（容器内已安装）
find_program(CCACHE_PROGRAM ccache CACHE)
if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}" CACHE FILEPATH "C compiler launcher")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}" CACHE FILEPATH "CXX compiler launcher")
endif()
