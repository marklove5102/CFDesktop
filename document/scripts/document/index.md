# document

> 文档编写日期: 2026-04-06

## 概述

本目录包含 CFDesktop 项目 MkDocs 文档系统的开发环境管理脚本。这些脚本用于自动化 Python 虚拟环境的创建、依赖安装、MkDocs 开发服务器的启停，以及 API 文档（Doxygen + Doxybook2）的生成管线。

## 目录结构

```
scripts/document/
├── mkdocs_dev.sh        # Linux/macOS Bash 脚本（主入口）
├── mkdocs_dev.ps1       # Windows PowerShell 脚本（对等实现）
└── pyproject.toml       # Python 依赖声明文件
```

## 快速开始

### Linux / macOS

```bash
# 首次安装环境并启动开发服务器
./scripts/document/mkdocs_dev.sh serve

# 自定义端口启动
./scripts/document/mkdocs_dev.sh serve -p 3000
```

### Windows

```powershell
# 首次安装环境并启动开发服务器
.\scripts\document\mkdocs_dev.ps1 serve

# 自定义端口启动
.\scripts\document\mkdocs_dev.ps1 serve -Port 3000
```

## 脚本一览

| 脚本 | 平台 | 功能 |
|------|------|------|
| `mkdocs_dev.sh` | Linux/macOS | Bash 版本，提供全功能 MkDocs 开发工作流 |
| `mkdocs_dev.ps1` | Windows | PowerShell 版本，与 Bash 版本功能完全对等 |
| `pyproject.toml` | 通用 | Python 依赖声明，包含 mkdocs 及所有插件 |

## 工作原理

脚本执行以下自动化流程：

```
检查 Python >= 3.10
    ↓
检测 .venv/ 是否存在
    ├── 不存在 → python3 -m venv .venv
    └── 已存在 → 跳过创建
    ↓
激活虚拟环境
    ↓
检查依赖是否需要更新（通过 hash 比对 pyproject.toml）
    ├── 需要更新 → pip install
    └── 无变更 → 跳过安装
    ↓
执行用户指定的子命令（serve / build / api 等）
```

## 相关配置文件

| 文件 | 位置 | 说明 |
|------|------|------|
| `mkdocs.yml` | 项目根目录 | MkDocs 主配置文件 |
| `Doxyfile` | 项目根目录 | Doxygen 配置文件（`api` 子命令使用） |
| `doxybook.json` | 项目根目录 | Doxybook2 配置文件（`api` 子命令使用） |

## 子命令对比

| 子命令 | 功能 | 是否需要虚拟环境 | 说明 |
|--------|------|-----------------|------|
| `serve` | 启动开发服务器 | 自动创建 | 默认命令，支持热重载 |
| `build` | 构建静态站点 | 自动创建 | 输出到 `out/docs/site/` |
| `api` | 生成 API 文档 | 自动创建 | 需要 doxygen + doxybook2 |
| `install` | 仅安装依赖 | 自动创建 | 用于环境初始化 |
| `clean` | 清理构建产物 | 不需要 | 保留 `.venv` |
| `reset` | 重建虚拟环境 | 删除后重建 | 完全重置开发环境 |

---

*Last updated: 2026-04-06*
