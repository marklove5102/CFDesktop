# Phase 0: 工程骨架搭建 TODO

> **状态**: 🚧 进行中
> **预计周期**: 1~2 周
> **依赖阶段**: 无
> **目标交付物**: 完整的 CMake 构建体系、目录结构、跨平台 CI/CD

---

## 一、阶段目标

### 核心目标
建立项目的工程化基础，确保后续所有开发工作在稳固的架构上进行。

### 具体交付物
- [ ] 完整的 CMake 构建体系
- [ ] 三层模块目录结构（base/sdk/shell）
- [ ] 跨平台 CI/CD 流水线
- [ ] 开发环境配置指南
- [ ] 代码规范与格式化配置

---

## 二、Week 1 任务

### Day 1-2: 目录结构创建
- [ ] 创建完整目录树
  - [ ] `src/base/` - 基础库源码
  - [ ] `src/sdk/` - SDK 层源码
  - [ ] `src/shell/` - Shell UI 源码
  - [ ] `src/simulator/` - 模拟器源码
  - [ ] `include/CFDesktop/` - 公共头文件
  - [ ] `tests/` - 测试代码
  - [ ] `cmake/` - CMake 模块
  - [ ] `assets/` - 资源文件
- [ ] 编写主 [CMakeLists.txt](../../CMakeLists.txt)
  - [ ] 设置 C++23 标准
  - [ ] 配置 Qt6 依赖
  - [ ] 添加子目录
- [ ] 配置 [CMakePresets.json](../../CMakePresets.json)
  - [ ] `linux-default` - 本地开发
  - [ ] `linux-arm-sf` - ARMv7 软渲染
  - [ ] `linux-arm-hf` - ARM64 硬件加速
- [ ] 创建各子模块的 CMakeLists.txt 框架

### Day 3-4: 交叉编译配置
- [ ] 编写 ARMv7 工具链文件 `cmake/toolchains/arm-linux-gnueabihf.cmake`
  - [ ] 设置 CMAKE_SYSTEM_NAME 为 Linux
  - [ ] 配置 arm-linux-gnueabihf-gcc/g++
  - [ ] 设置 Qt6 路径
- [ ] 编写 ARM64 工具链文件 `cmake/toolchains/aarch64-linux-gnu.cmake`
  - [ ] 设置 CMAKE_SYSTEM_PROCESSOR 为 aarch64
  - [ ] 配置 aarch64-linux-gnu-gcc/g++
  - [ ] 设置 Qt6 ARM64 路径
- [ ] 配置 Qt6 路径查找逻辑
- [ ] 测试交叉编译（可用 Docker 验证）
  - [ ] 验证 ARMv7 编译
  - [ ] 验证 ARM64 编译

### Day 5: 代码规范配置
- [ ] 配置 [.clang-format](../../.clang-format)
  - [ ] 基于 Google 风格
  - [ ] 设置缩进为 4 空格
  - [ ] 配置列宽 100
  - [ ] 设置指针对齐方式
- [ ] 配置 [.clang-tidy](../../.clang-tidy)
  - [ ] 启用 modernize 检查
  - [ ] 配置性能相关检查
- [ ] 编写代码格式化脚本 `tools/format.sh`
- [ ] 配置 Git pre-commit hook
  - [ ] 创建 `.git-hooks/pre-commit` 脚本
  - [ ] 自动格式化检查 (clang-format --dry-run)
  - [ ] 静态分析 (clang-tidy)
  - [ ] 添加 trailing whitespace 检查
  - [ ] 配置可跳过选项 (git commit --no-verify)
  - [ ] 安装脚本配置 (`tools/install-hooks.sh`)

---

## 三、Week 2 任务

### Day 1-3: CI/CD 搭建
- [ ] 创建 Docker 构建镜像
  - [ ] `docker/builder-linux-x64/Dockerfile`
  - [ ] `docker/builder-arm-sf/Dockerfile`
  - [ ] 配置 Qt6 环境
- [ ] 编写 GitHub Actions 工作流 `.github/workflows/build.yml`
  - [ ] `build-linux-x64` 作业
  - [ ] `build-arm-sf` 交叉编译作业
  - [ ] `code-quality` 静态分析作业
  - [ ] 配置 artifact 上传
- [ ] 配置部署流程 `.github/workflows/deploy.yml`
  - [ ] 设备选择输入
  - [ ] SSH 部署配置
- [ ] 测试完整构建流程

### Day 4-5: 开发工具完善
- [ ] 创建 VSCode 工作区配置 `.vscode/settings.json`
  - [ ] CMake 配置参数
  - [ ] 代码格式化保存
  - [ ] Clang 配置
- [ ] 配置推荐扩展 `.vscode/extensions.json`
  - [ ] CMake Tools
  - [ ] C/C++
  - [ ] clang-format
- [ ] 编写开发环境设置文档 `docs/development/SETUP.md`
- [ ] 创建应用模板框架 `templates/app-template/`
- [ ] 编写 Hello World 测试程序

---

## 四、验收标准

### 构建系统
- [ ] `cmake --preset linux-default && cmake --build build --preset linux-default` 成功
- [ ] `cmake --preset linux-arm-sf && cmake --build build --preset linux-arm-sf` 产出 ARM 二进制
- [ ] 所有目标编译无警告

### CI/CD
- [ ] Push 代码自动触发构建
- [ ] 构建失败发送通知
- [ ] 测试用例自动运行
- [ ] Artifact 正确上传

### 开发环境
- [ ] VSCode 能够正确索引所有符号
- [ ] 代码格式化一键执行
- [ ] 远程调试配置可用
- [ ] 新团队成员能在 30 分钟内完成环境搭建

---

## 五、风险与缓解

| 风险 | 影响 | 缓解措施 |
|------|------|----------|
| Qt6 交叉编译配置复杂 | 延迟 2-3 天 | 使用 Docker 预配置环境 |
| CI 资源不足 | 构建时间长 | 优化依赖缓存，使用矩阵策略 |
| 工具链版本兼容性 | 编译失败 | 固定 Docker 镜像版本 |

---

## 六、相关文档

- 设计文档: [../design_stage/00_phase0_project_skeleton.md](../design_stage/00_phase0_project_skeleton.md)
- 主 CMakeLists.txt: [../../CMakeLists.txt](../../CMakeLists.txt)
- CMake 预设: [../../CMakePresets.json](../../CMakePresets.json)

---

*最后更新: 2026-03-05*
