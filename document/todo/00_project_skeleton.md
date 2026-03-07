# Phase 0: 工程骨架搭建 TODO

> **状态**: ✅ 已完成 (100%)
> **预计周期**: 1~2 周
> **依赖阶段**: 无
> **目标交付物**: 完整的 CMake 构建体系、目录结构、Git Hooks CI/CD

---

## 一、阶段目标

### 核心目标
建立项目的工程化基础，确保后续所有开发工作在稳固的架构上进行。

### 具体交付物
- [x] 完整的 CMake 构建体系
- [x] 模块化目录结构（base/ui/example/test）
- [x] Git Hooks CI/CD 策略
- [x] 开发环境配置 (VSCode)
- [x] 代码规范与格式化配置

---

## 二、Week 1 任务

### Day 1-2: 目录结构创建
- [x] 创建完整目录树
  - [x] `base/` - 基础库源码 (system/, utilities/)
  - [x] `ui/` - UI 框架源码 (core/, components/, widget/)
  - [x] `example/` - 示例程序
  - [x] `test/` - 测试代码
  - [x] `cmake/` - CMake 模块
  - [x] `scripts/` - 构建和辅助脚本
- [x] 编写主 [CMakeLists.txt](../../CMakeLists.txt)
  - [x] 设置 C++17 标准
  - [x] 配置 Qt6 依赖
  - [x] 添加子目录
  - [x] 自动化 MOC/RCC/UIC
- [x] 创建各子模块的 CMakeLists.txt 框架
- [x] 配置 VSCode 开发环境
  - [x] `.vscode/settings.json` - clangd 配置
  - [x] `.vscode/extensions.json` - 推荐扩展
  - [x] `.vscode/launch.json` - 调试配置

### Day 3-4: ~~交叉编译配置~~ (已取消，使用 Docker 多架构替代)
- ~~编写 ARMv7 工具链文件~~
- ~~编写 ARM64 工具链文件~~
- [x] Docker 多架构构建替代方案
  - [x] `scripts/docker/Dockerfile.build` - 支持 amd64/arm64/armhf

### Day 5: 代码规范配置
- [x] 配置 [.clang-format](../../.clang-format)
  - [x] 基于 LLVM 风格
  - [x] 设置缩进为 4 空格
  - [x] 配置列宽 100
  - [x] 设置指针对齐方式
- [x] 配置 Git pre-commit hook
  - [x] 创建 `scripts/release/hooks/pre-commit.sample` 脚本
  - [x] 自动格式化 (clang-format)
  - [x] 添加 trailing whitespace 检查
  - [x] 配置可跳过选项 (git commit --no-verify)
  - [x] 安装脚本配置 (`scripts/release/hooks/install_hooks.sh`)

---

## 三、Week 2 任务

### Day 1-3: CI/CD 搭建
- [x] 创建 Docker 构建镜像
  - [x] `scripts/docker/Dockerfile.build` - 多架构支持 (amd64/arm64/armhf)
  - [x] 配置 Qt6 环境 (aqtinstall 6.8.1)
- [x] 编写 Git Hooks 工作流
  - [x] `scripts/release/hooks/pre-push.sample` - Push 前验证
  - [x] `scripts/release/hooks/pre-commit.sample` - Commit 前检查
  - [x] `scripts/release/hooks/version_utils.sh` - 版本工具
  - [x] `scripts/release/hooks/install_hooks.sh` - Hooks 安装脚本
- [x] Docker 构建脚本
  - [x] `scripts/build_helpers/docker_start.sh` - 本地 Docker 构建
  - [x] `scripts/build_helpers/ci_build_entry.sh` - CI 入口 (自动架构检测)
- [x] 配置部署流程 `.github/workflows/deploy.yml` - MkDocs 文档自动部署
- [x] 测试完整构建流程

### Day 4-5: 开发工具完善
- [x] 创建 VSCode 工作区配置 `.vscode/settings.json`
  - [x] CMake 配置参数
  - [x] Clangd 配置
- [x] 配置推荐扩展 `.vscode/extensions.json`
  - [x] CMake Tools
  - [x] C/C++
  - [x] clang-format
- [x] 编写 Hello World 测试程序
  - [x] example/gui/theme/ - Material Gallery 示例
  - [x] example/base/system/ - 系统信息示例
  - [x] example/ui/widget/material/ - 控件示例
- [x] 编写开发环境设置文档
  - [x] [document/development/README.md](../development/README.md) - 开发环境总览
  - [x] [document/development/01_prerequisites.md](../development/01_prerequisites.md) - 前置要求
  - [x] [document/development/02_quick_start.md](../development/02_quick_start.md) - 快速开始
  - [x] [document/development/03_build_system.md](../development/03_build_system.md) - 构建系统
  - [x] [document/development/04_development_tools.md](../development/04_development_tools.md) - 开发工具
  - [x] [document/development/05_docker_build.md](../development/05_docker_build.md) - Docker 构建
  - [x] [document/development/06_git_hooks.md](../development/06_git_hooks.md) - Git Hooks
  - [x] [document/development/07_troubleshooting.md](../development/07_troubleshooting.md) - 常见问题

---

## 四、验收标准

### CI/CD
- [x] Push 代码前自动触发本地构建验证 (pre-push hook)
- [x] Commit 前自动代码格式检查 (pre-commit hook)
- [x] 版本号检查 (阻止未更新版本的推送)
- [x] 多架构构建支持 (amd64/arm64/armhf)
- [x] 代码格式化一键执行 (pre-commit hook 自动格式化)
- [x] MkDocs 文档自动部署到 gh-pages

### 开发环境
- [x] VSCode 能够正确索引所有符号 (clangd 配置)
- [x] 代码格式化一键执行 (pre-commit hook)
- [x] 调试配置可用 (.vscode/launch.json 自动生成)
- [x] 新团队成员能在 30 分钟内完成环境搭建 (document/development/)

---

## 五、风险与缓解

| 风险 | 影响 | 缓解措施 |
|------|------|----------|
| Docker 多架构构建时间较长 | 开发效率 | 使用 --fast-build 缓存镜像 |
| ARM64 QEMU 仿真速度慢 | CI 耗时 | 仅在 release/Major 版本验证 |
| 工具链版本兼容性 | 编译失败 | 固定 Docker 镜像版本 |

---

## 六、相关文档

- 开发环境设置: [development/](../development/) - 完整的开发环境文档
- 设计文档: [../design_stage/00_phase0_project_skeleton.md](../design_stage/00_phase0_project_skeleton.md)
- 参考文档: [done/00_project_skeleton_ref.md](done/00_project_skeleton_ref.md)
- 主 CMakeLists.txt: [../../CMakeLists.txt](../../CMakeLists.txt)

---

*最后更新: 2026-03-07*
