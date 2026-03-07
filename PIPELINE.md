# CFDesktop 跨平台构建流水线 — 分步实施指南

> **文档用途**：本文档提供跨平台构建流水线的详细分步实施指南。每个阶段可以独立完成和验证，方便团队逐步推进。
>
> **核心设计**：利用现有的 `check_toolchain.cmake` 机制，只创建工具链文件，不修改现有脚本。使用**多架构 Docker 容器**实现跨平台测试。

---

## 目录

- [架构概览](#架构概览)
- [Phase 1: 创建 Linux 工具链文件](#phase-1-创建-linux-工具链文件)
- [Phase 2: 创建 Docker 构建环境](#phase-2-创建-docker-构建环境)
- [Phase 3: 创建 CI 构建入口](#phase-3-创建-ci-构建入口)
- [Phase 4: 配置 GitHub Actions](#phase-4-配置-github-actions)
- [Phase 5: 实现异步合并机制](#phase-5-实现异步合并机制)
- [附录：现有系统分析](#附录现有系统分析)

---

## 架构概览

### 设计目标

不管开发者在何种平台（Windows/Linux/macOS），本地修改代码后 push 时：

1. **立即提交**：push 到临时分支，不阻塞开发者
2. **异步验证**：GitHub Actions 在**ARM64 Docker 容器**中进行纯净环境编译和测试
3. **自动合并**：验证通过后自动合并到目标分支

### 系统架构

```
┌─────────────────────────────────────────────────────────────────────────┐
│                           开发者本地                                      │
│  ┌─────────────┐     ┌──────────────┐     ┌──────────────────────────┐  │
│  │  Windows    │     │    Linux     │     │        macOS             │  │
│  │  代码编辑    │     │   代码编辑    │     │       代码编辑           │  │
│  └──────┬──────┘     └──────┬───────┘     └──────────┬───────────────┘  │
│         │                    │                        │                  │
│         │  ┌─────────────────┴─────────────────────────┐                │
│         │  │   本地多架构验证 (可选)                    │                │
│         │  │   - amd64: docker run --platform linux/amd64              │
│         │  │   - arm64: docker run --platform linux/arm64 (QEMU)       │
│         │  └───────────────────────────────────────────┘                │
│         └────────────────────┴────────────────────────┘                  │
│                              │                                           │
│                         git push                                         │
│                              ↓                                           │
│              ┌─────────────────────────────────┐                        │
│              │  pre-push hook (轻量检查)        │                        │
│              │  - 基本语法检查                  │                        │
│              │  - 提交到临时分支                │                        │
│              │    ci-verify/xxx-timestamp      │                        │
│              │  - 立即返回（非阻塞）            │                        │
│              └─────────────────────────────────┘                        │
└─────────────────────────────────────────────────────────────────────────┘
                              │
                              ↓
┌─────────────────────────────────────────────────────────────────────────┐
│                        GitHub Actions CI                                  │
│  ┌────────────────────────────────────────────────────────────────────┐ │
│  │  触发条件：push 到 ci-verify/* 分支                                │ │
│  │                                                                     │ │
│  │  Job: ARM64 原生编译和测试                                          │ │
│  │  ├─ docker run --platform linux/arm64                             │ │
│  │  ├─ 镜像：ubuntu:24.04 ARM64 + Qt6 + GCC                          │ │
│  │  ├─ QEMU 仿真运行（在 x86_64 runner 上）                           │ │
│  │  ├─ 挂载代码                                                       │ │
│  │  ├─ cmake -DUSE_TOOLCHAIN=linux/ci（原生编译）                      │ │
│  │  ├─ 构建                                                           │ │
│  │  └─ 运行测试（在 ARM64 容器中运行）                                │ │
│  │                                                                     │ │
│  │  验证成功 → 触发自动合并                                            │ │
│  └────────────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────────────┘
                              │
                              ↓
┌─────────────────────────────────────────────────────────────────────────┐
│                        自动合并流程                                       │
│  ┌────────────────────────────────────────────────────────────────────┐ │
│  │  1. 临时分支: ci-verify/feat-xxx-20240306-123456                   │ │
│  │  2. 目标分支: feat/xxx 或 main                                     │ │
│  │  3. 验证成功后:                                                    │ │
│  │     - 自动创建 PR 或直接合并                                        │ │
│  │     - 删除临时分支                                                  │ │
│  │     - 通知开发者                                                    │ │
│  └────────────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────────────┘
```

### 利用现有机制

**现有 `check_toolchain.cmake` 已经实现了工具链选择**：

```
INI 配置：toolchain=linux/ci
         ↓
CMake 参数：-DUSE_TOOLCHAIN=linux/ci
         ↓
自动查找：cmake/cmake_toolchain/linux/ci-toolchain.cmake
         ↓
自动设置：CMAKE_TOOLCHAIN_FILE
```

**结论**：只需创建工具链文件，不需要修改任何脚本！

### 为什么使用多架构 Docker 容器？

本方案采用**多架构 Docker 原生容器**方式，使用 `--platform` 参数在对应的架构容器中运行原生编译：

| 对比项 | 交叉编译方案 | 多架构容器方案（本方案） |
|--------|---------------|------------------------|
| ARM 测试 | ❌ 跳过（无法在 x86_64 主机运行） | ✅ 在 ARM64 容器中运行 |
| 工具链复杂度 | ❌ 需要交叉编译工具链 | ✅ 使用原生工具链 |
| 测试真实性 | ⚠️ 无法验证 ARM 产物是否正确运行 | ✅ 真实 ARM 环境测试 |
| 配置复杂度 | ⚠️ 需要配置 sysroot 等 | ✅ 使用相同的工具链文件 |
| CI 构建（x86 runner） | ✅ 交叉编译较快 | ⚠️ QEMU 仿真较慢 |

#### 本地验证 vs CI 环境

| 环境 | ARM64 构建方式 | 说明 |
|------|---------------|------|
| 本地 (x86_64) | `--platform linux/arm64` + QEMU | 可验证 ARM64 产物正确性 |
| 本地 (ARM64) | `--platform linux/arm64` 原生 | 快速原生编译 |
| CI (x86_64 runner) | `--platform linux/arm64` + QEMU | 较慢但可接受，产生原生 ARM64 二进制 |
| CI (ARM64 runner) | `--platform linux/arm64` 原生 | 最优方案（如自托管 runner） |

**结论**：
- **本地验证**：开发者可选择 amd64 或 arm64 进行验证
- **CI 环境**：只编译 ARM64，使用 QEMU 仿真（速度可接受）

---

## Phase 1: 创建 CI 专用工具链文件

### 目标

创建专门面向 CI 构建的精简工具链文件，利用现有的 `check_toolchain.cmake` 机制。

### 设计原则

1. **CI 专用**：工具链文件专门为 CI 环境设计，不适用于本地开发
2. **精简高效**：仅使用 GCC，去掉 Clang 支持以节约构建时间
3. **多架构兼容**：支持 AMD64 和 ARM64 容器的原生编译
4. **零修改**：利用现有机制，不需要修改任何脚本

### 为什么不需要修改脚本？

现有的 `check_toolchain.cmake` 已经能够：
1. 解析 `platform/toolchain` 格式
2. 自动查找对应的工具链文件
3. 自动设置 `CMAKE_TOOLCHAIN_FILE`

只需要在正确位置创建工具链文件即可。

### 操作步骤

#### Step 1.1: 创建目录结构

```bash
# 在项目根目录执行
mkdir -p cmake/cmake_toolchain/linux
```

**目录结构预期**：
```
cmake/
└── cmake_toolchain/
    ├── windows/                        # 已存在
    │   ├── llvm-toolchain.cmake
    │   └── gcc-toolchain.cmake
    └── linux/                          # 新建
        ├── ci-x86_64-toolchain.cmake   # AMD64 CI 工具链
        ├── ci-aarch64-toolchain.cmake  # ARM64 CI 工具链
        └── ci-armhf-toolchain.cmake    # ARM32 CI 工具链
```

**注意**：
- 采用分离的架构特定工具链方案，每个架构有独立的工具链文件
- 容器内根据架构自动选择对应的工具链文件
- Qt 路径根据架构不同而不同（gcc_64/gcc_arm64）

#### 工具链文件内容示例

每个工具链文件的结构类似，主要区别在于 Qt 路径：

```cmake
# =============================================================================
# CI Build Toolchain Configuration (AMD64/ARM64)
# =============================================================================
# 专用工具链文件，用于 Docker 容器内的 CI 构建

set(CMAKE_SYSTEM_NAME Linux)

# -----------------------------------------------------------------------------
# Qt6 搜索路径（容器环境）
# Docker 容器通过 aqtinstall 安装 Qt 到 /opt/Qt/6.8.1/
# AMD64: /opt/Qt/6.8.1/gcc_64
# ARM64: /opt/Qt/6.8.1/gcc_arm64
# -----------------------------------------------------------------------------

set(QT6_BASE_DIR "/opt/Qt/6.8.1/<arch>")
set(QT6_CMAKE_DIR "${QT6_BASE_DIR}/lib/cmake/Qt6")

# 允许环境变量覆盖（用于调试）
if(DEFINED ENV{Qt6_DIR})
    set(QT6_CMAKE_DIR "$ENV{Qt6_DIR}")
elseif(DEFINED ENV{QT6_DIR})
    set(QT6_BASE_DIR "$ENV{QT6_DIR}")
    set(QT6_CMAKE_DIR "${QT6_BASE_DIR}/lib/cmake/Qt6")
endif()

# 设置 Qt6_DIR 和 CMAKE_PREFIX_PATH
set(Qt6_DIR "${QT6_CMAKE_DIR}" CACHE PATH "Qt6 installation directory")
set(CMAKE_PREFIX_PATH "${QT6_BASE_DIR}")

# -----------------------------------------------------------------------------
# 编译器配置
# 使用系统默认的 GCC
# -----------------------------------------------------------------------------

# 启用 ccache 加速构建（容器内已安装）
find_program(CCACHE_PROGRAM ccache CACHE)
if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()
```

### 验证方法

```bash
# 1. 检查文件是否创建成功
ls -la cmake/cmake_toolchain/linux/

# 2. 测试工具链解析（不实际构建）
cmake -DUSE_TOOLCHAIN=linux/ci-x86_64 -S . -B /tmp/test_build
cmake -DUSE_TOOLCHAIN=linux/ci-aarch64 -S . -B /tmp/test_build

# 应该看到：
# -- Toolchain shorthand 'linux/ci-x86_64' resolved to:
# --   .../cmake/cmake_toolchain/linux/ci-x86_64-toolchain.cmake

# 3. 清理测试目录
rm -rf /tmp/test_build
```

### 预期结果

- CMake 能正确解析 `linux/ci-x86_64` 和 `linux/ci-aarch64` 工具链简写
- 找到对应的工具链文件
- 配置过程无错误
- ccache 被正确启用

### 常见问题

| 问题 | 原因 | 解决方案 |
|------|------|----------|
| CMake 找不到工具链文件 | 文件名或路径不正确 | 确认文件名格式为 `ci-toolchain.cmake` |
| Qt6 not found | CMAKE_PREFIX_PATH 不正确 | 根据实际 Qt6 安装路径调整 |
| ccache 未启用 | 容器内未安装 ccache | 检查 Dockerfile 中是否安装 ccache |

### 实施状态

**状态**: ✅ 已完成 (2026-03-06)

**已创建文件**:
- `cmake/cmake_toolchain/linux/ci-x86_64-toolchain.cmake` - AMD64 CI 工具链（Qt: /opt/Qt/6.8.1/gcc_64）
- `cmake/cmake_toolchain/linux/ci-aarch64-toolchain.cmake` - ARM64 CI 工具链（Qt: /opt/Qt/6.8.1/gcc_arm64）
- `cmake/cmake_toolchain/linux/ci-armhf-toolchain.cmake` - ARM32 CI 工具链（IMX6ULL）
- `document/ci/README.md` - CI/CD 架构概述文档
- `document/ci/toolchain-setup.md` - 工具链设置指南

**配置更新**:
- `cmake/cmake_toolchain/linux/.gitignore` - 已添加工具链文件排除规则

---

## Phase 2: 创建 Docker 构建环境

### 目标

创建支持多架构的 Dockerfile，定义纯净的 Linux 构建环境。

### 为什么使用 Docker？

1. **环境隔离**：不依赖本地环境
2. **可重现**：每次构建都是全新的环境
3. **跨平台**：Windows/Mac/Linux 都能运行相同的容器
4. **多架构支持**：通过 `--platform` 参数指定目标架构

### 操作步骤

#### Step 2.1: 创建 Dockerfile.build

**文件路径**: `Dockerfile.build`

```dockerfile
# =============================================================================
# CFDesktop Build Environment - Multi-Architecture Support
# =============================================================================
# 纯净的 Linux 构建环境，用于 CI 和本地验证
#
# 构建多架构镜像：
#   docker buildx build --platform linux/amd64,linux/arm64 \
#     -f Dockerfile.build -t cfdesktop-build .
#
# 单架构构建：
#   docker build -t cfdesktop-build -f Dockerfile.build .
#
# 使用方式：
#   docker run --rm --platform linux/amd64 -v $(pwd):/project cfdesktop-build
#   docker run --rm --platform linux/arm64 -v $(pwd):/project cfdesktop-build
# =============================================================================

FROM ubuntu:22.04

# 避免交互式提示
ENV DEBIAN_FRONTEND=noninteractive

# =============================================================================
# 安装基础构建工具
# =============================================================================
RUN apt-get update && apt-get install -y \
    # 基础工具
    build-essential \
    cmake \
    ninja-build \
    ccache \
    git \
    wget \
    curl \
    # Qt6 依赖
    qt6-base-dev \
    qt6-tools-dev \
    qt6-declarative-dev \
    qt6-svg-dev \
    # 清理缓存
    && rm -rf /var/lib/apt/lists/*

# =============================================================================
# 配置 ccache
# =============================================================================
RUN ccache --max-size=5G --set-config=compiler_check=%compiler%2S -s

# =============================================================================
# 设置工作目录
# =============================================================================
WORKDIR /project

# =============================================================================
# 默认命令
# =============================================================================
CMD ["/bin/bash"]
```

#### Step 2.2: 创建 .dockerignore

**文件路径**: `.dockerignore`

```
# Git 相关
.git
.gitignore
.gitattributes

# 构建产物
out/
build/
*.o
*.a
*.so
*.dylib
*.dll
*.exe

# IDE 相关
.vscode/
.vs/
.idea/
*.user
*.suo

# 临时文件
*.tmp
*.log
*.swp
*~

# 操作系统
.DS_Store
Thumbs.db

# 文档
docs/
*.md
!README.md
```

#### Step 2.3: 创建 docker-compose.yml（可选，便于本地测试）

**文件路径**: `docker-compose.yml`

```yaml
version: '3.8'

services:
  # AMD64 构建环境
  build-amd64:
    build:
      context: .
      dockerfile: Dockerfile.build
    image: cfdesktop-build:amd64
    platform: linux/amd64
    volumes:
      - .:/project
    working_dir: /project
    command: /bin/bash

  # ARM64 构建环境（需要支持 ARM 的 Docker）
  build-arm64:
    build:
      context: .
      dockerfile: Dockerfile.build
    image: cfdesktop-build:arm64
    platform: linux/arm64
    volumes:
      - .:/project
    working_dir: /project
    command: /bin/bash

  # 快速验证服务（默认 AMD64）
  verify:
    build:
      context: .
      dockerfile: Dockerfile.build
    image: cfdesktop-build:latest
    platform: linux/amd64
    volumes:
      - .:/project
    working_dir: /project
    command: bash scripts/build_helpers/ci_build_entry.sh ci
```

### 验证方法

```bash
# 1. 构建镜像（AMD64）
docker build -t cfdesktop-build -f Dockerfile.build .

# 2. 测试容器运行（AMD64）
docker run --rm --platform linux/amd64 -v $(pwd):/project cfdesktop-build

# 在容器内执行：
#   which cmake
#   which gcc
#   qmake6 --version

# 3. 构建多架构镜像（需要 buildx）
docker buildx create --name cfdesktop-multi \
  --platform linux/amd64,linux/arm64 \
  -f Dockerfile.build .

# 4. 使用 docker-compose（如果有）
docker-compose run verify
```

### 预计产出
Dockerfile.build
docker-compose.yml和对应的 ignore 文件
scripts下要有快速的Windows/Linux启动脚本，脚本有注释维护（注意需要检查是否有docker，没有的话需要提醒安装）


### 预期结果

- 镜像构建成功
- 容器内能找到所有必要的工具
- 能在容器内完成构建

### 常见问题

| 问题 | 原因 | 解决方案 |
|------|------|----------|
| Windows 路径挂载问题 | Git Bash 和 Docker 路径格式不同 | 使用 PowerShell 或 CMD，或调整路径格式 |
| 容器内权限问题 | Linux 容器内文件权限 | 考虑添加用户配置或使用 `--user` 参数 |
| Qt6 找不到 | Ubuntu 22.04 默认源 Qt6 版本 | 使用官方 Qt 安装脚本或调整版本 |
| ARM64 容器无法启动 | Docker 不支持 ARM 模拟 | 确保启用 Docker 的实验性功能 |

### 实施状态

**状态**: ✅ 已完成 (2026-03-07)

**已创建文件**:
- `scripts/docker/Dockerfile.build` - 多架构 Docker 镜像定义
- `scripts/docker/.dockerignore` - Docker 忽略规则
- `scripts/docker/docker-compose.yml` - Docker Compose 配置
- `scripts/build_helpers/docker_start.sh` - Linux Docker 启动脚本
- `scripts/build_helpers/docker_start.ps1` - Windows Docker 启动脚本

**实际实现说明**:
- 基于 Ubuntu 24.04（文档设计为 22.04）
- 使用 aqtinstall 自动安装 Qt 6.8.1
- 支持多种运行模式：交互式 shell、CI 验证、快速构建
- 包含完整的日志记录功能

**详细文档**: [Phase 2 实施完成文档](../document/ci/phase2-docker-env.md)

---

## Phase 3: 创建 CI 构建入口

### 目标

创建统一的 CI 构建入口脚本，在 Docker 容器中执行构建。

### 为什么需要单独的 CI 入口？

1. **独立配置**：CI 使用独立的配置文件和构建目录
2. **环境适配**：自动检测是否在容器内运行
3. **统一接口**：Docker 和 GitHub Actions 使用相同的入口

### 操作步骤

#### Step 3.1: 创建 build_ci_config.ini

**文件路径**: `scripts/build_helpers/build_ci_config.ini`

```ini
# CI Build Configuration
# 用于 GitHub Actions 和 Docker 容器内的构建

[cmake]
# CMake 生成器（CI 优先使用 Ninja）
generator=Ninja
# 工具链配置（CI 专用，容器内原生编译）
toolchain=linux/ci
# 构建类型
build_type=Release

[paths]
# 源码目录（相对于项目根目录）
source=.
# 构建输出目录（独立于开发构建）
build_dir=out/build_ci

[build]
# 并行任务数（留空则自动检测，通常使用 nproc）
parallel_jobs=

[toolchain]
# 目标平台（仅用于记录，实际由容器架构决定）
target=linux
```

#### Step 3.2: 创建 ci_build_entry.sh

**文件路径**: `scripts/build_helpers/ci_build_entry.sh`

```bash
#!/bin/bash
# =============================================================================
# CI Build Entry Point
# =============================================================================
#
# Usage:
#   bash ci_build_entry.sh [ci|develop]
#
# Arguments:
#   ci      - 使用 build_ci_config.ini（默认）
#   develop - 使用 build_develop_config.ini
#
# Examples:
#   bash ci_build_entry.sh ci
#   docker run --rm -v $(pwd):/project cfdesktop-build bash ci_build_entry.sh ci
#
# =============================================================================

set -e

# =============================================================================
# 共享函数定义
# =============================================================================

log() {
    local message="$1"
    local level="$2"
    local timestamp=$(date "+%Y-%m-%d %H:%M:%S")
    local color=""

    case "$level" in
        INFO)    color="\033[0;36m" ;;    # Cyan
        SUCCESS) color="\033[0;32m" ;;    # Green
        WARNING) color="\033[0;33m" ;;    # Yellow
        ERROR)   color="\033[0;31m" ;;    # Red
        *)       color="\033[0m" ;;
    esac

    echo -e "${color}[$timestamp] [$level] $message\033[0m"
}

get_ini_config() {
    local filepath="$1"
    local current_section=""
    local config=""

    if [[ ! -f "$filepath" ]]; then
        log "Configuration file not found: $filepath" "ERROR"
        exit 1
    fi

    while IFS= read -r line || [[ -n "$line" ]]; do
        line=$(echo "$line" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')

        if [[ -z "$line" ]] || [[ "$line" =~ ^[#\;] ]]; then
            continue
        fi

        if [[ "$line" =~ ^\[([^\]]+)\]$ ]]; then
            current_section="${BASH_REMATCH[1]}"
            continue
        fi

        if [[ "$line" =~ ^([^=]+)=(.*)$ ]]; then
            local key=$(echo "${BASH_REMATCH[1]}" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')
            local value=$(echo "${BASH_REMATCH[2]}" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')

            if [[ -n "$current_section" ]]; then
                config+="config_${current_section}_${key}=\"$value\""$'\n'
            fi
        fi
    done < "$filepath"

    echo "$config"
}

# =============================================================================
# 参数处理
# =============================================================================

CONFIG_MODE="${1:-ci}"

log "========================================" "INFO"
log "CI Build Entry (Config: $CONFIG_MODE)" "INFO"
log "========================================" "INFO"

# =============================================================================
# 路径设置
# =============================================================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

log "Project root: $PROJECT_ROOT" "INFO"
cd "$PROJECT_ROOT"

# =============================================================================
# 选择配置文件
# =============================================================================

case "$CONFIG_MODE" in
    ci)
        CONFIG_FILE="$SCRIPT_DIR/build_ci_config.ini"
        if [[ ! -f "$CONFIG_FILE" ]]; then
            log "build_ci_config.ini not found, falling back to develop config" "WARNING"
            CONFIG_FILE="$SCRIPT_DIR/build_develop_config.ini"
        fi
        ;;
    develop)
        CONFIG_FILE="$SCRIPT_DIR/build_develop_config.ini"
        ;;
    *)
        log "Unknown config mode: $CONFIG_MODE (use 'ci' or 'develop')" "ERROR"
        exit 1
        ;;
esac

log "Loading configuration from: $CONFIG_FILE" "INFO"
eval "$(get_ini_config "$CONFIG_FILE")"

# =============================================================================
# 解析配置
# =============================================================================

BUILD_DIR="${config_paths_build_dir:-out/build_ci}"
SOURCE_DIR="${config_paths_source:-.}"
GENERATOR="${config_cmake_generator:-Ninja}"
BUILD_TYPE="${config_cmake_build_type:-Release}"
TOOLCHAIN="${config_cmake_toolchain:-linux/ci}"

# 解析路径
if [[ "$SOURCE_DIR" = /* ]]; then
    RESOLVED_SOURCE_DIR="$SOURCE_DIR"
else
    RESOLVED_SOURCE_DIR="$PROJECT_ROOT/$SOURCE_DIR"
fi

FULL_BUILD_PATH="$PROJECT_ROOT/$BUILD_DIR"

# 检测当前架构
ARCH=$(uname -m)
log "Detected architecture: $ARCH" "INFO"

# =============================================================================
# 1. 清理构建目录
# =============================================================================

log "========================================" "INFO"
log "Step 1: Cleaning build directory" "INFO"
log "========================================" "INFO"

if [[ -d "$FULL_BUILD_PATH" ]]; then
    log "Removing: $FULL_BUILD_PATH" "INFO"
    rm -rf "$FULL_BUILD_PATH"
fi
mkdir -p "$FULL_BUILD_PATH"

# =============================================================================
# 2. CMake 配置
# =============================================================================

log "========================================" "INFO"
log "Step 2: Configuring with CMake" "INFO"
log "========================================" "INFO"

log "Generator: $GENERATOR" "INFO"
log "Toolchain: $TOOLCHAIN" "INFO"
log "Build type: $BUILD_TYPE" "INFO"
log "Source: $RESOLVED_SOURCE_DIR" "INFO"
log "Build: $FULL_BUILD_PATH" "INFO"

cmake -G "$GENERATOR" \
      -DUSE_TOOLCHAIN="$TOOLCHAIN" \
      -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
      -S "$RESOLVED_SOURCE_DIR" \
      -B "$FULL_BUILD_PATH"

log "Configuration successful!" "SUCCESS"

# =============================================================================
# 3. 构建项目
# =============================================================================

log "========================================" "INFO"
log "Step 3: Building project" "INFO"
log "========================================" "INFO"

PARALLEL_JOBS="${config_build_parallel_jobs:-$(nproc)}"
log "Building with $PARALLEL_JOBS parallel jobs" "INFO"

cmake --build "$FULL_BUILD_PATH" --parallel "$PARALLEL_JOBS"

log "Build successful!" "SUCCESS"

# =============================================================================
# 4. 运行测试
# =============================================================================

log "========================================" "INFO"
log "Step 4: Running tests" "INFO"
log "========================================" "INFO"

TEST_DIR="$FULL_BUILD_PATH/test"

if [[ -d "$TEST_DIR" ]]; then
    if ctest --test-dir "$TEST_DIR" --output-on-failure; then
        log "All tests passed!" "SUCCESS"
    else
        exit_code=$?
        log "Some tests failed with exit code: $exit_code" "WARNING"
        # 不退出，仅警告
    fi
else
    log "Test directory not found: $TEST_DIR" "WARNING"
    log "Project may not have tests configured yet" "INFO"
fi

# =============================================================================
# 完成
# =============================================================================

log "========================================" "INFO"
log "CI Build completed successfully!" "SUCCESS"
log "========================================" "INFO"
```

#### Step 3.3: 设置执行权限

```bash
chmod +x scripts/build_helpers/ci_build_entry.sh
```

### 验证方法

```bash
# 在 Docker 容器内测试

# 1. 测试 AMD64 容器
docker run --rm --platform linux/amd64 \
    -v $(pwd):/project \
    cfdesktop-build \
    bash scripts/build_helpers/ci_build_entry.sh ci

# 2. 测试 ARM64 容器（需要支持 ARM 的 Docker）
docker run --rm --platform linux/arm64 \
    -v $(pwd):/project \
    cfdesktop-build \
    bash scripts/build_helpers/ci_build_entry.sh ci

# 3. 验证 ARM64 产物架构
docker run --rm --platform linux/arm64 \
    -v $(pwd):/project \
    cfdesktop-build \
    bash -c "bash scripts/build_helpers/ci_build_entry.sh ci && \
             file out/build_ci/bin/* | grep 'ARM aarch64'"
```

### 预期结果

- 脚本能完成完整的构建流程
- AMD64 和 ARM64 容器都能正常运行
- ARM64 容器内的产物是 ARM 架构
- 测试能够在对应架构的容器中运行

### 实施状态

**状态**: ✅ 已完成 (2026-03-07)

**已创建文件**:
- `scripts/build_helpers/build_ci_config.ini` - CI 构建配置
- `scripts/build_helpers/ci_build_entry.sh` - CI 构建入口脚本

**功能特性**:
- 支持 CI 模式（完整构建+测试）和测试模式
- 彩色日志输出（INFO、SUCCESS、WARNING、ERROR）
- 完善的错误处理机制
- 调用现有的 linux_develop_build.sh 和 linux_run_tests.sh

**详细文档**: [Phase 3 实施完成文档](../document/ci/phase3-ci-entry.md)

---

## Phase 4: Git Hooks 本地验证机制

### 目标

实现 Git hooks（pre-commit 和 pre-push），在本地进行代码质量检查和构建验证，确保远程 main 分支始终保持可构建状态。

### 为什么是本地验证而非远程 CI？

1. **即时反馈**：开发者立即知道代码是否有问题
2. **远程干净**：远程仓库只保留经过验证的代码
3. **简单高效**：不需要维护临时分支和异步合并逻辑

### 分支策略

| 分支类型 | 命名规则 | 用途 | 是否验证构建 |
|----------|----------|------|-------------|
| **main** | - | 开发主分支，始终保持可构建状态 | ✅ 是（pre-push） |
| **release** | `release/x.y` | 发布分支（Major.Minor） |  ✅ 是（pre-push）阅读scripts\release\README.md获知不同发行策略时，CI检查的严格程度哈|
| **feat** | 本地临时 | 开发新功能，不推送到远程 | - |

**工作流程**：
```
本地: feat分支开发 → 合并到本地main → git push（触发Docker验证）→ 推送成功
远程: 只有main + release/*分支，保持干净
```

### 核心设计：远程保护 + 本地验证

```
┌─────────────────────────────────────────────────────────────┐
│ 验证失败时的状态                                            │
├─────────────────────────────────────────────────────────────┤
│  本地main: A---B---C (已合并feat，被验证失败阻止)            │
│  远程main:  A---B (保持干净，未受影响)                       │
│                                                             │
│  回退方法: git reset --hard origin/main                     │
└─────────────────────────────────────────────────────────────┘
```

**关键原则**：远程main永远保持可构建状态

### 钩子设计

| Hook | 触发时机 | 触发条件 | 检查内容 | 失败行为 |
|------|----------|----------|----------|----------|
| **pre-commit** | 每次`git commit` | 所有分支 | 代码格式检查、空白字符检查 | 阻止提交 |
| **pre-push** | 每次`git push` | 仅`main`分支 | Docker容器内完整构建+测试 | 阻止推送 |

### 操作步骤

#### Step 4.1: 创建 `scripts/release/hooks/pre-commit.sample`

**文件路径**: `scripts/release/hooks/pre-commit.sample`

```bash
#!/bin/bash
# =============================================================================
# Git Pre-Commit Hook - 本地代码质量检查
# =============================================================================
#
# 安装方法: bash scripts/release/hooks/install_hooks.sh
#
# 检查内容:
#   1. 空白字符问题（Git内置）
#   2. C++代码格式（需要clang-format）
#
# 绕过方法: git commit --no-verify -m "message"
# =============================================================================

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m'

echo -e "${GREEN}=== Pre-Commit Hook ===${NC}"

# 获取脚本目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

# 1. 基本空白字符检查（Git内置）
if ! git diff-index --check --cached HEAD -- 2>/dev/null; then
    echo -e "${RED}❌ 空白字符检查失败${NC}"
    echo "请修复尾随空格和混合缩进问题"
    exit 1
fi
echo -e "${GREEN}✓ 空白字符检查通过${NC}"

# 2. C++格式检查（如果clang-format可用）
if command -v clang-format >/dev/null 2>&1; then
    cd "$PROJECT_ROOT"
    if bash scripts/develop/format_cpp.sh --check; then
        echo -e "${GREEN}✓ C++格式检查通过${NC}"
    else
        echo -e "${RED}❌ C++格式检查失败${NC}"
        echo "运行 'bash scripts/develop/format_cpp.sh' 自动修复"
        exit 1
    fi
else
    echo -e "${YELLOW}⚠️  clang-format未安装，跳过格式检查${NC}"
    echo "安装方法: sudo apt install clang-format"
fi

echo -e "${GREEN}=== Pre-Commit 检查完成 ===${NC}"
exit 0
```

#### Step 4.2: 创建 `scripts/release/hooks/pre-push.sample`

**文件路径**: `scripts/release/hooks/pre-push.sample`

```bash
#!/bin/bash
# =============================================================================
# Git Pre-Push Hook - 本地Docker构建验证
# =============================================================================
#
# 安装方法: bash scripts/release/hooks/install_hooks.sh
#
# 行为:
#   - 仅在main分支推送时运行完整验证
#   - 其他分支跳过Docker验证
#
# 绕过方法: git push --no-verify
# =============================================================================

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# 获取当前分支
CURRENT_BRANCH=$(git symbolic-ref --short HEAD 2>/dev/null || echo "")

# 只在main分支运行完整验证
if [[ "$CURRENT_BRANCH" != "main" ]]; then
    echo -e "${BLUE}=== Pre-Push Hook ===${NC}"
    echo "当前分支: ${YELLOW}$CURRENT_BRANCH${NC}，跳过Docker验证（仅main分支需要）"
    exit 0
fi

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}=== Pre-Push Hook (main分支验证) ===${NC}"
echo -e "${BLUE}========================================${NC}"

# 获取脚本目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
cd "$PROJECT_ROOT"

# 检测Docker是否可用
if ! command -v docker >/dev/null 2>&1; then
    echo -e "${RED}❌ Docker未安装或未运行${NC}"
    echo "请安装Docker: https://www.docker.com/products/docker-desktop"
    echo -e "${YELLOW}或使用 --no-verify 跳过此检查: git push --no-verify${NC}"
    exit 1
fi

# 检测Docker daemon是否运行
if ! docker info &> /dev/null; then
    echo -e "${RED}❌ Docker daemon未运行${NC}"
    echo "请启动Docker Desktop"
    echo -e "${YELLOW}或使用 --no-verify 跳过此检查: git push --no-verify${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Docker检测通过${NC}"
echo ""
echo -e "${BLUE}正在验证 main 分支的可构建性...${NC}"
echo ""

# 运行CI验证（使用fast-build模式复用镜像）
if bash scripts/build_helpers/docker_start.sh --verify --fast-build; then
    echo ""
    echo -e "${GREEN}========================================${NC}"
    echo -e "${GREEN}=== 验证通过，可以推送 ===${NC}"
    echo -e "${GREEN}========================================${NC}"
    exit 0
else
    exit_code=$?
    echo ""
    echo -e "${RED}========================================${NC}"
    echo -e "${RED}=== 验证失败，推送被阻止 ===${NC}"
    echo -e "${RED}========================================${NC}"
    echo ""
    echo "回退方法:"
    echo "  git reset --hard origin/main"
    echo ""
    echo -e "${YELLOW}或使用 --no-verify 强制推送（不推荐）${NC}"
    exit $exit_code
fi
```

#### Step 4.3: 创建 `scripts/release/hooks/install_hooks.sh`

**文件路径**: `scripts/release/hooks/install_hooks.sh`

```bash
#!/bin/bash
# =============================================================================
# Git Hooks 安装脚本
# =============================================================================
#
# 用法: bash scripts/release/hooks/install_hooks.sh
#
# 此脚本将钩子安装到 .git/hooks/ 目录
# =============================================================================

set -e

GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
HOOKS_DIR="$PROJECT_ROOT/.git/hooks"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Git Hooks 安装${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# 检查是否存在.git目录
if [[ ! -d "$PROJECT_ROOT/.git" ]]; then
    echo "错误: 未找到.git目录，请确认你在Git仓库中"
    exit 1
fi

# 复制钩子
echo "安装 pre-commit 钩子..."
cp "$SCRIPT_DIR/pre-commit.sample" "$HOOKS_DIR/pre-commit"
chmod +x "$HOOKS_DIR/pre-commit"
echo -e "${GREEN}✓ pre-commit 已安装${NC}"

echo ""
echo "安装 pre-push 钩子..."
cp "$SCRIPT_DIR/pre-push.sample" "$HOOKS_DIR/pre-push"
chmod +x "$HOOKS_DIR/pre-push"
echo -e "${GREEN}✓ pre-push 已安装${NC}"

echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Git Hooks 安装完成！${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo "已安装的钩子:"
echo "  - pre-commit:  代码格式检查（所有分支）"
echo "  - pre-push:    Docker构建验证（仅main分支）"
echo ""
echo "验证安装:"
echo "  ls -la .git/hooks/pre-commit .git/hooks/pre-push"
```

#### Step 4.4: 创建 `scripts/release/hooks/install_hooks.ps1`

**文件路径**: `scripts/release/hooks/install_hooks.ps1`

```powershell
# =============================================================================
# Git Hooks 安装脚本 (Windows)
# =============================================================================
#
# 用法: .\scripts\release\hooks\install_hooks.ps1
# =============================================================================

$ErrorActionPreference = "Stop"

$Green = "`e[0;32m"
$Blue = "`e[0;34m"
$Reset = "`e[0m"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Split-Path -Parent (Split-Path -Parent $ScriptDir)
$HooksDir = "$ProjectRoot\.git\hooks"

Write-Host ""
Write-Host "$Blue========================================$Reset"
Write-Host "$Blue Git Hooks 安装 (Windows)$Reset"
Write-Host "$Blue========================================$Reset"
Write-Host ""

# 检查是否存在.git目录
if (-not (Test-Path "$ProjectRoot\.git")) {
    Write-Host "错误: 未找到.git目录" -ForegroundColor Red
    exit 1
}

# 复制钩子
Write-Host "安装 pre-commit 钩子..."
Copy-Item "$ScriptDir\pre-commit.sample" "$HooksDir\pre-commit" -Force
Write-Host "$Green✓ pre-commit 已安装$Reset"

Write-Host ""
Write-Host "安装 pre-push 钩子..."
Copy-Item "$ScriptDir\pre-push.sample" "$HooksDir\pre-push" -Force
Write-Host "$Green✓ pre-push 已安装$Reset"

Write-Host ""
Write-Host "$Green========================================$Reset"
Write-Host "$Green Git Hooks 安装完成！$Reset"
Write-Host "$Green========================================$Reset"
Write-Host ""
Write-Host "已安装的钩子:"
Write-Host "  - pre-commit:  代码格式检查（所有分支）"
Write-Host "  - pre-push:    Docker构建验证（仅main分支）"
```

#### Step 4.5: 创建发布规则文档

**目录结构**:
```
document/release_rule/
├── README.md                    # 发布规则总览
├── branch_strategy.md           # 分支策略说明
├── git_hooks_guide.md           # Git hooks使用指南
└── github_settings.md           # GitHub远程保护设置
```

**document/release_rule/README.md 内容概要**:
- 项目分支策略说明
- 钩子安装命令
- 常见问题解答

**document/release_rule/branch_strategy.md 内容概要**:
- main分支的作用和保护
- release分支的命名规范
- feat分支的使用方式

**document/release_rule/git_hooks_guide.md 内容概要**:
- pre-commit检查内容说明
- pre-push验证流程说明
- 验证失败后的处理方法
- 绕过方法及注意事项

**document/release_rule/github_settings.md 内容概要**:
- 如何设置main分支为protected
- 配置状态检查（可选）
- 配置规则示例

### 工作流程说明

```
开发者工作流程：

1. 创建feat分支开发新功能
   $ git checkout -b feat/my-feature
   # ... 编码 ...

2. 提交代码（触发pre-commit检查）
   $ git add .
   $ git commit -m "feat: add new feature"
   # pre-commit检查代码格式

3. 合并到main分支
   $ git checkout main
   $ git merge feat/my-feature

4. 推送到远程（触发pre-push验证）
   $ git push
   # pre-push在Docker容器中运行完整构建+测试
   # 验证通过 → 推送成功
   # 验证失败 → 推送被阻止，远程main保持干净

5. 如果验证失败，回退本地main
   $ git reset --hard origin/main
   # 修复问题后重新合并
```

### 验证方法

#### 安装钩子
```bash
bash scripts/release/hooks/install_hooks.sh
```

#### 验证pre-commit
```bash
# 故意制造格式错误的代码
echo "int x;" >> src/test.cpp
git add src/test.cpp
git commit -m "test"
# 应该被阻止，提示格式问题
```

#### 验证pre-push（仅main分支）
```bash
git checkout main
git merge feat/my-feature
git push
# 会触发Docker验证
# 验证通过 → 推送成功
# 验证失败 → 推送被阻止
```

#### 验证非main分支不触发完整验证
```bash
git checkout release/1.0
git push
# 应该输出: "当前分支: release/1.0，跳过Docker验证"
```

### 预期结果

- pre-commit在所有分支上检查代码格式
- pre-push在main和release分支上运行Docker构建验证（release分支自动检测验证级别）
- 验证失败时推送被阻止，远程main保持干净
- 开发者可以安全地回退本地main分支

### 实施状态

**状态**: ✅ 已完成 (2026-03-07)

**已创建文件**:
- `scripts/release/hooks/pre-commit.sample` - 代码格式检查钩子（空白字符 + C++ 格式）
- `scripts/release/hooks/pre-push.sample` - Docker 构建验证钩子（支持 main 和 release 分支）
- `scripts/release/hooks/version_utils.sh` - 版本号解析辅助函数
- `scripts/release/hooks/install_hooks.sh` - Linux/macOS 安装脚本
- `scripts/release/hooks/install_hooks.ps1` - Windows PowerShell 安装脚本
- `scripts/release/hooks/README.md` - 快速参考文档
- `document/release_rule/git_hooks_guide.md` - 完整使用指南

**功能特性**:
- **pre-commit**: 仅格式检查（不含构建），快速响应
- **pre-push**:
  - main 分支: X64 FastBuild + Tests
  - release 分支: 自动检测 Major/Minor/Patch 版本变化，调整验证级别
    - Major: X64 + ARM64 完整构建
    - Minor: X64 完整构建
    - Patch: X64 FastBuild + Tests
  - feat 分支: 跳过验证
- 支持绕过（`--no-verify`）
- 自动备份现有自定义钩子
- 彩色输出和详细日志

**使用方法**:
```bash
# 安装钩子
bash scripts/release/hooks/install_hooks.sh

# 验证安装
ls -la .git/hooks/pre-commit .git/hooks/pre-push
```

**详细文档**: [Git Hooks 使用指南](document/release_rule/git_hooks_guide.md)

---

## 附录：现有系统分析

### A.1 现有工具链选择机制

项目使用 `cmake/check_toolchain.cmake` 实现工具链选择：

```
配置文件：toolchain=linux/ci
    ↓
CMake：cmake -DUSE_TOOLCHAIN=linux/ci
    ↓
check_toolchain.cmake 解析
    ↓
查找：cmake/cmake_toolchain/linux/ci-toolchain.cmake
    ↓
设置：CMAKE_TOOLCHAIN_FILE
```

### A.2 现有脚本结构

```
scripts/build_helpers/
├── build_develop_config.ini     # Windows 开发配置
├── build_deploy_config.ini      # Windows 发布配置
├── build_develop_config.ini.template  # Linux 模板配置
├── linux_configure.sh           # Linux 配置脚本
├── linux_fast_develop_build.sh  # Linux 快速开发构建
├── linux_run_tests.sh           # Linux 测试脚本
└── (Windows .ps1 脚本...)
```

### A.3 不需要修改的原因

1. **工具链选择已实现**：`check_toolchain.cmake` 已经处理所有逻辑
2. **配置驱动**：只需要在 INI 中指定不同的 toolchain
3. **向后兼容**：新增工具链文件不影响现有功能

### A.4 需要新增的文件

| 文件 | 说明 |
|------|------|
| `cmake/cmake_toolchain/linux/ci-toolchain.cmake` | CI 专用 GCC 工具链（AMD64/ARM64） |
| `scripts/build_helpers/build_ci_config.ini` | CI 配置 |
| `scripts/build_helpers/ci_build_entry.sh` | CI 入口脚本 |
| `scripts/build_helpers/pre-push.sample` | Pre-push hook |
| `Dockerfile.build` | Docker 多架构镜像定义 |
| `.github/workflows/build.yml` | GitHub Actions |
| `docker-compose.yml` | Docker Compose（可选） |

---

## 总结

本文档提供了跨平台构建流水线的完整实施方案，核心特点：

1. **利用现有机制**：通过 `check_toolchain.cmake`，只需新增工具链文件
2. **多架构 Docker**：使用 `--platform` 参数支持不同架构的原生容器编译
3. **CI 只编译 ARM64**：使用 QEMU 仿真，产生真正的 ARM64 二进制文件
4. **本地可选验证**：开发者可选择 amd64 或 arm64 进行本地验证
5. **异步非阻塞**：开发者 push 后立即继续工作
6. **自动合并**：CI 通过后自动创建 PR

建议实施顺序：
1. Phase 1: 创建工具链文件
2. Phase 2: 创建 Docker 环境
3. Phase 3: 创建 CI 入口
5. Phase 4: 实现异步合并

每个 Phase 完成后进行验证，确保问题及时发现和解决。

---

*文档版本：v5.2 | 最后更新：2026-03-07 | 维护者：CFDesktop 团队*

**v5.2 变更 (2026-03-07)**：
- Phase 5 改为 Git Hooks 本地验证机制
- 添加分支策略说明（main + release/* + feat本地）
- 添加 pre-commit 和 pre-push 钩子的完整实施步骤
- 移除异步远程CI方案（临时分支+自动合并）

**v5.1 变更**：
- 更新为分离的架构特定工具链方案（ci-x86_64/ci-aarch64/ci-armhf）
- Phase 4 标记为跳过（不在远端构建）
- 文档重命名为描述性命名（toolchain-setup.md, docker-environment.md, ci-build-entry.md）
- 删除过时的 arm64-cross-compile.md 文档

**v5.0 变更**：
- 方案从交叉编译改为多架构 Docker 原生容器
- 本地验证支持 amd64 和 arm64
- 脚本参数从 `aarch64` 改为 `arm64`
