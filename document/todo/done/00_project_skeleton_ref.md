# Phase 0: 工程骨架搭建 - 参考文档

> **模块状态**: 进行中
> **完成度**: 70%
> **更新日期**: 2026-03-05
> **说明**: 基础构建系统已就绪，部分开发工具待完善

---

## 一、模块概述

工程骨架模块是 CFDesktop 项目的基础设施层，负责提供完整的构建系统、开发环境配置和 CI/CD 流水线。该模块为所有后续开发工作提供稳固的工程化基础。

### 核心职责

1. **CMake 构建系统** - 跨平台编译、依赖管理、输出配置
2. **代码规范配置** - 格式化、静态分析、编码标准
3. **开发工具集成** - IDE 配置、调试支持、辅助脚本
4. **CI/CD 流水线** - 自动构建、文档部署、质量检查

---

## 二、当前实现状态

### 总体完成度: 70%

| 项目 | 完成度 | 说明 |
|------|--------|------|
| CMake 构建系统 | 90% | 主配置完成，缺少交叉编译工具链 |
| 代码规范配置 | 60% | .clang-format 已配置，缺少 .clang-tidy |
| 开发工具配置 | 70% | VSCode/clangd 配置完成，缺少格式化脚本 |
| CI/CD 流水线 | 50% | 文档部署已完成，缺少构建流水线 |
| 交叉编译支持 | 0% | 未实现 |

---

## 三、已完成项清单

### 3.1 CMake 构建系统

**主配置文件**: `CMakeLists.txt`

**已完成功能**:
- CMake 3.16+ 版本要求
- C++17 标准设置
- Qt6 集成 (Core, Gui, Widgets)
- 自动化 MOC/RCC/UIC 处理
- 构建日志辅助系统
- 编译命令导出 (compile_commands.json)
- 分类输出目录配置 (bin/, lib/, examples/)
- VSCode clangd 配置自动生成
- VSCode 调试配置自动生成

**关键配置摘要**:
```cmake
cmake_minimum_required(VERSION 3.16)
project(CFDesktop VERSION 0.0.1 LANGUAGES CXX)

# C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6 依赖
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# 子模块
add_subdirectory(base)   # 基础库
add_subdirectory(ui)     # UI 框架
add_subdirectory(example) # 示例程序
add_subdirectory(test)    # 测试代码
```

**依赖文件**:
- `cmake/build_log_helper.cmake` - 构建日志辅助
- `cmake/check_toolchain.cmake` - 工具链检查
- `cmake/custom_target_helper.cmake` - 自定义目标辅助
- `cmake/OutputDirectoryConfig.cmake` - 输出目录配置
- `cmake/generate_develop_helpers.cmake` - 开发辅助生成
- `cmake/ExampleLauncher.cmake` - Windows 启动脚本生成
- `cmake/QtDeployUtils.cmake` - Qt 部署工具

### 3.2 代码规范配置

**配置文件**: `.clang-format`

**已完成功能**:
- 基于 LLVM 风格
- 4 空格缩进
- 100 字符列宽
- 左对齐指针/引用 (int* a)
- 大括号附加风格 (Attach)
- 包含块排序
- C++17 标准

**配置摘要**:
```yaml
BasedOnStyle: LLVM
IndentWidth: 4
UseTab: Never
ColumnLimit: 100
PointerAlignment: Left
BreakBeforeBraces: Attach
Standard: c++17
SortIncludes: true
```

### 3.3 开发工具配置

**VSCode 配置**: `.vscode/settings.json`

**已完成功能**:
- clangd 路径配置
- 编译命令目录指定
- 查询驱动器配置
- 后台索引启用
- clang-tidy 集成
- 头文件插入优化

**其他 VSCode 配置**:
- `.vscode/extensions.json` - 推荐扩展
- `.vscode/launch.json` - 调试配置 (自动生成)

### 3.4 CI/CD 流水线

**部署流水线**: `.github/workflows/deploy.yml`

**已完成功能**:
- MkDocs 文档自动构建
- Doxygen API 文档生成
- doxybook2 Markdown 转换
- GitHub Pages 自动部署
- Python 3.11 环境配置

**触发条件**:
- Push 到 main 分支
- 手动触发 (workflow_dispatch)

### 3.5 目录结构

**已建立的目录结构**:
```
CFDesktop/
├── base/           # 基础库模块
│   ├── system/     # 系统检测 (CPU, 内存)
│   └── include/    # 公共头文件
├── ui/             # UI 框架
│   ├── core/       # 核心组件
│   ├── material/   # Material Design
│   └── base/       # 基础工具
├── example/        # 示例程序
├── test/           # 测试代码
├── cmake/          # CMake 模块
├── .github/        # GitHub 配置
└── .vscode/        # VSCode 配置
```

---

## 四、待完成项清单

### 4.1 .clang-tidy 静态分析配置 (0%)

**需求描述**:
- 启用 modernize 检查
- 配置性能相关检查
- 添加自定义规则

**建议文件路径**: `.clang-tidy`

**配置示例**:
```yaml
---
InheritParentConfig: true
Checks: >
  -*,
  modernize-*,
  performance-*,
  readability-*,
  -modernize-use-trailing-return-type
WarningsAsErrors: ''
HeaderFilterRegex: '.*'
```

### 4.2 格式化脚本 (0%)

**需求描述**:
- 一键格式化所有代码
- 格式检查 (CI 用)
- 支持增量格式化

**建议文件路径**: `tools/format.sh`

**脚本示例**:
```bash
#!/bin/bash
# format.sh - 代码格式化脚本

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

if [[ "$1" == "--check" ]]; then
    # 检查模式
    find . -name "*.cpp" -o -name "*.h" | xargs clang-format --dry-run -Werror
else
    # 格式化模式
    find . -name "*.cpp" -o -name "*.h" | xargs clang-format -i
fi
```

### 4.3 Git pre-commit hooks (0%)

**需求描述**:
- 创建 `.git-hooks/pre-commit` 脚本
- 自动格式化检查 (clang-format --dry-run)
- 静态分析 (clang-tidy)
- 添加 trailing whitespace 检查
- 配置可跳过选项 (git commit --no-verify)
- 安装脚本配置 (`tools/install-hooks.sh`)

**建议文件路径**:
- `.git-hooks/pre-commit` - pre-commit 脚本
- `tools/install-hooks.sh` - 安装脚本

**pre-commit 脚本示例**:
```bash
#!/bin/bash
set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

echo "Running pre-commit checks..."

# 检查格式化
echo "Checking code formatting..."
CHANGED_FILES=$(git diff --cached --name-only --diff-filter=ACM | grep -E '\.(cpp|h|cc|hh)$' || true)
if [ -n "$CHANGED_FILES" ]; then
    echo "$CHANGED_FILES" | xargs clang-format --dry-run -Werror
fi

# 检查尾随空格
echo "Checking trailing whitespace..."
git diff --cached --name-only --diff-filter=ACM | xargs grep -l '[[:space:]]$' && exit 1 || true

echo "Pre-commit checks passed!"
```

**安装脚本示例**:
```bash
#!/bin/bash
# tools/install-hooks.sh
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

# 创建 .git/hooks 目录
mkdir -p .git/hooks

# 复制 pre-commit hook
cp .git-hooks/pre-commit .git/hooks/pre-commit
chmod +x .git/hooks/pre-commit

echo "Git hooks installed successfully!"
```

### 4.4 构建流水线 (0%)

**需求描述**:
- 多平台构建 (Linux, Windows)
- 交叉编译 (ARM)
- 自动测试
- Artifact 上传

**建议文件路径**: `.github/workflows/build.yml`

**流水线结构**:
```yaml
name: Build Matrix

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  build-linux-x64:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Configure CMake
        run: cmake -B build -DCMAKE_BUILD_TYPE=Release
      - name: Build
        run: cmake --build build --parallel
      - name: Test
        run: ctest --test-dir build

  build-windows:
    runs-on: windows-latest
    steps: # ...
```

### 4.5 交叉编译工具链 (0%)

**需求描述**:
- ARMv7 (IMX6ULL) 工具链
- ARM64 (RK3568/RK3588) 工具链
- Qt6 交叉编译配置

**建议文件路径**:
- `cmake/toolchains/arm-linux-gnueabihf.cmake`
- `cmake/toolchains/aarch64-linux-gnu.cmake`

**ARMv7 工具链示例**:
```cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)

set(CMAKE_FIND_ROOT_PATH /opt/arm-sfm-toolchain)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(Qt6_DIR /opt/qt6-arm/lib/cmake/Qt6)
```

---

## 五、已取消/调整项

### 5.1 CMakePresets.json - 已取消

**原因**:
- 不方便维护
- 配置与命令行参数重复
- 团队更习惯使用 -DCMAKE_* 参数

**替代方案**:
- 使用 build_*.config.ini 配置文件
- 通过命令行参数传递构建配置

### 5.2 src/base/sdk/shell 三层结构 - 已调整

**原计划**:
```
src/
├── base/    # 基础库
├── sdk/     # SDK 层
└── shell/   # Shell UI
```

**实际采用**:
```
base/        # 基础库
ui/          # UI 框架
example/     # 示例程序
```

**原因**: 更简洁的模块划分

### 5.3 ARM 交叉编译工具链 - 已推迟

**原因**: 优先实现桌面平台功能，嵌入式平台后续支持

---

## 六、完成度百分比

| 项目 | 原计划需求 | 已实现 | 完成度 |
|------|-----------|--------|--------|
| CMake 构建系统 | 100% | 90% | 90% |
| 代码规范配置 | 100% | 60% | 60% |
| 开发工具配置 | 100% | 70% | 70% |
| CI/CD 流水线 | 100% | 50% | 50% |
| 交叉编译支持 | 100% | 0% | 0% |
| **总体** | **100%** | **70%** | **70%** |

---

## 七、关键文件路径

### 已实现文件

```
CFDesktop/
├── CMakeLists.txt                    # 主 CMake 配置
├── .clang-format                     # 代码格式化配置
├── .vscode/
│   ├── settings.json                 # VSCode 设置
│   ├── extensions.json               # 推荐扩展
│   └── launch.json                   # 调试配置 (自动生成)
├── .github/
│   └── workflows/
│       └── deploy.yml                # 文档部署流水线
└── cmake/
    ├── build_log_helper.cmake        # 构建日志
    ├── check_toolchain.cmake         # 工具链检查
    ├── custom_target_helper.cmake    # 自定义目标
    ├── OutputDirectoryConfig.cmake   # 输出配置
    ├── generate_develop_helpers.cmake # 开发辅助生成
    ├── ExampleLauncher.cmake         # 启动脚本生成
    └── QtDeployUtils.cmake           # Qt 部署工具
```

### 待创建文件

```
CFDesktop/
├── .clang-tidy                       # 静态分析配置
├── tools/
│   └── format.sh                     # 格式化脚本
├── .git/
│   └── hooks/
│       └── pre-commit                # Git pre-commit hook
├── .github/
│   └── workflows/
│       └── build.yml                 # 构建流水线
└── cmake/
    └── toolchains/
        ├── arm-linux-gnueabihf.cmake # ARMv7 工具链
        └── aarch64-linux-gnu.cmake   # ARM64 工具链
```

---

## 八、下一步行动建议

### 优先级1 (高)

1. **添加 .clang-tidy 配置**
   - 优先级: 最高
   - 理由: 提高代码质量
   - 预计工作量: 1天

2. **创建格式化脚本**
   - 优先级: 高
   - 理由: 统一代码格式
   - 预计工作量: 1天

3. **配置 Git pre-commit hooks**
   - 优先级: 高
   - 理由: 防止不规范代码提交
   - 预计工作量: 1天

### 优先级2 (中)

4. **创建构建流水线**
   - 优先级: 中
   - 理由: 自动化构建和测试
   - 预计工作量: 2-3天

5. **编写开发环境设置文档**
   - 优先级: 中
   - 理由: 帮助新成员快速上手
   - 预计工作量: 1天

### 优先级3 (低)

6. **添加交叉编译工具链**
   - 优先级: 低
   - 理由: 嵌入式平台后续支持
   - 预计工作量: 3-4天

---

## 九、相关文档

- 原始TODO: [../00_project_skeleton.md](../00_project_skeleton.md)
- 设计文档: [../../design_stage/00_phase0_project_skeleton.md](../../design_stage/00_phase0_project_skeleton.md)
- Base库参考: [./02_base_library_ref.md](./02_base_library_ref.md)

---

*最后更新: 2026-03-05*
