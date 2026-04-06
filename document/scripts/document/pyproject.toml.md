# pyproject.toml

> 文档编写日期: 2026-04-06

## 使用办法 (Usage)

### 文件位置

```
scripts/document/pyproject.toml
```

### 安装依赖

```bash
# 方式 1: 通过开发脚本（推荐）
./scripts/document/mkdocs_dev.sh install

# 方式 2: 手动安装（需要先激活 .venv）
source .venv/bin/activate
pip install -e scripts/document/
```

```powershell
# Windows PowerShell
.\scripts\document\mkdocs_dev.ps1 install

# 或手动安装
. .venv\Scripts\Activate.ps1
pip install -e scripts\document\
```

## 详解 (Detailed Explanation)

### 文件用途

`pyproject.toml` 是 Python 项目的现代配置文件标准（PEP 517/518），用于声明 CFDesktop 文档开发环境所需的 Python 依赖。`mkdocs_dev.sh` 和 `mkdocs_dev.ps1` 脚本通过 `pip install -e` 命令读取此文件来安装依赖。

### 完整内容

```toml
[project]
name = "cfdesktop-docs"
version = "0.1.0"
description = "CFDesktop documentation development environment"
requires-python = ">=3.10"
dependencies = [
    "mkdocs>=1.5.0",
    "mkdocs-material>=9.5.0",
    "mkdocs-awesome-pages-plugin>=2.9.0",
    "mkdocs-git-revision-date-localized-plugin>=1.2.0",
]
```

### 字段说明

| 字段 | 值 | 说明 |
|------|-----|------|
| `name` | `cfdesktop-docs` | 项目名称，用于 pip 包标识 |
| `version` | `0.1.0` | 版本号 |
| `description` | `CFDesktop documentation ...` | 项目描述 |
| `requires-python` | `>=3.10` | 最低 Python 版本要求 |
| `dependencies` | (列表) | 运行时依赖列表 |

### 依赖包详解

| 包名 | 最低版本 | 用途 | 项目中使用位置 |
|------|----------|------|---------------|
| `mkdocs` | >= 1.5.0 | MkDocs 静态站点生成器本体 | `mkdocs serve`, `mkdocs build` |
| `mkdocs-material` | >= 9.5.0 | Material Design 主题，提供现代化 UI | `mkdocs.yml` → `theme: material` |
| `mkdocs-awesome-pages-plugin` | >= 2.9.0 | 灵活的页面组织插件 | `mkdocs.yml` → `plugins: awesome-pages` |
| `mkdocs-git-revision-date-localized-plugin` | >= 1.2.0 | 自动显示文章创建和更新时间 | `mkdocs.yml` → `plugins: git-revision-date-localized` |

### 与 CI 环境的关系

CI 环境 (`.github/workflows/deploy.yml`) 直接使用 `pip install` 安装相同的包：

```yaml
# CI 中的安装方式
- name: 安装依赖
  run: |
    pip install mkdocs-material
    pip install mkdocs-awesome-pages-plugin
    pip install mkdocs-git-revision-date-localized-plugin
```

本地开发环境通过 `pyproject.toml` 统一管理版本约束，确保 CI 和本地依赖一致。

### 版本约束说明

使用 `>=` 约束而非固定版本（`==`），原因：

1. **兼容性** — 允许在不修改配置的情况下获取 bugfix 更新
2. **安全性** — 允许获取安全补丁
3. **CI 稳定性** — CI 环境总是安装最新兼容版本

如需固定版本，可创建 `requirements.lock` 文件（当前未使用）。

### 修改此文件

当需要添加或升级依赖时：

1. 编辑 `scripts/document/pyproject.toml` 中的 `dependencies` 列表
2. 运行以下命令使变更生效：

```bash
# 方式 1: 通过脚本自动检测变更
./scripts/document/mkdocs_dev.sh install

# 方式 2: 强制重装
./scripts/document/mkdocs_dev.sh reset
```

**自动检测机制：** 开发脚本会比较 `pyproject.toml` 的 MD5 hash 与 `.venv/.deps_installed` 中存储的 hash，如不一致则自动触发依赖更新。

### 与其他配置文件的关系

```
pyproject.toml                  声明 Python 依赖
    ↓
mkdocs_dev.sh / mkdocs_dev.ps1  读取 pyproject.toml 安装依赖
    ↓
mkdocs.yml                      使用已安装的包配置站点
    ↓
document/                       文档源文件
```

### 注意事项

1. **版本约束** — 修改版本约束时注意与 CI 环境的兼容性
2. **依赖数量** — 当前仅包含 4 个直接依赖，MkDocs Material 会自动拉取其子依赖
3. **Python 版本** — `requires-python = ">=3.10"` 确保使用现代 Python 特性
4. **文件格式** — 遵循 TOML 规范，编辑时注意语法正确性
5. **可编辑安装** — 脚本使用 `pip install -e` 安装，修改 `pyproject.toml` 后可通过 hash 比对自动检测
