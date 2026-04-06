# mkdocs_dev.sh

> 文档编写日期: 2026-04-06

## 使用办法 (Usage)

### 基本语法

```bash
./scripts/document/mkdocs_dev.sh <command> [OPTIONS]
```

### 命令说明

| 命令 | 说明 |
|------|------|
| `serve` | 启动 MkDocs 开发服务器（默认命令） |
| `build` | 构建静态站点到 `out/docs/site/` |
| `api` | 运行 Doxygen + Doxybook2 API 文档管线 |
| `install` | 仅创建/更新虚拟环境和安装依赖 |
| `clean` | 清理构建产物（`out/docs/`、`__pycache__`、Doxygen XML） |
| `reset` | 删除并重建 `.venv` 虚拟环境 |
| `help` | 显示帮助信息 |

### 参数说明

| 参数 | 说明 |
|------|------|
| `-p`, `--port PORT` | 开发服务器端口（默认: `8000`） |
| `-b`, `--bind ADDR` | 开发服务器绑定地址（默认: `127.0.0.1`） |
| `-v`, `--verbose` | 启用详细输出模式 |
| `-h`, `--help` | 显示帮助信息 |

### 使用示例

```bash
# 启动开发服务器（默认 8000 端口）
./scripts/document/mkdocs_dev.sh serve

# 自定义端口和绑定地址启动
./scripts/document/mkdocs_dev.sh serve --port 3000 --bind 0.0.0.0

# 构建静态站点
./scripts/document/mkdocs_dev.sh build

# 生成 API 文档（需要 doxygen + doxybook2）
./scripts/document/mkdocs_dev.sh api

# 仅安装依赖（不启动服务器）
./scripts/document/mkdocs_dev.sh install

# 清理构建产物
./scripts/document/mkdocs_dev.sh clean

# 完全重置虚拟环境
./scripts/document/mkdocs_dev.sh reset

# 使用详细模式调试
./scripts/document/mkdocs_dev.sh install --verbose

# 查看帮助
./scripts/document/mkdocs_dev.sh help
```

## Scripts详解 (Detailed Explanation)

### 脚本用途

`mkdocs_dev.sh` 是 CFDesktop 项目的 MkDocs 文档开发环境管理脚本（Linux/macOS 版本）。它自动化了以下工作流：

1. **Python 虚拟环境管理** — 检测、创建、激活 `.venv`
2. **依赖安装** — 基于 `pyproject.toml` 安装 MkDocs 及插件
3. **开发服务器** — 一键启动 `mkdocs serve` 实时预览
4. **静态站点构建** — 输出到 `out/docs/site/`
5. **API 文档生成** — Doxygen + Doxybook2 完整管线
6. **环境维护** — 清理、重置等操作

### 脚本常量

| 常量 | 值 | 说明 |
|------|-----|------|
| `MIN_PYTHON_MAJOR` | `3` | 最低 Python 主版本号 |
| `MIN_PYTHON_MINOR` | `10` | 最低 Python 次版本号 |
| `VENV_DIR` | `.venv` | 虚拟环境目录名（位于项目根目录） |
| `DEPS_MARKER` | `.deps_installed` | 依赖安装标记文件（位于 `.venv/` 内） |
| `DOCS_OUTPUT_DIR` | `out/docs/site` | 静态站点输出路径 |
| `DOXYGEN_XML_DIR` | `xml` | Doxygen XML 输出目录 |
| `DOXYGEN_API_DIR` | `document/api` | API 文档 Markdown 输出目录 |

### 环境要求

| 依赖项 | 版本要求 | 用途 |
|--------|----------|------|
| **Python 3** | >= 3.10 | 运行 MkDocs 及相关工具 |
| **python3-venv** | 与 Python 版本一致 | 创建虚拟环境 |
| **pip** | 随 Python 安装 | 包管理 |
| **doxygen** | 任意稳定版 | API 文档生成（仅 `api` 命令需要） |
| **doxybook2** | >= 1.5.0 | Doxygen XML → Markdown 转换（仅 `api` 命令需要） |

### Python 安装指引

```bash
# Ubuntu / Debian
sudo apt install python3 python3-venv python3-pip

# Fedora / RHEL
sudo dnf install python3 python3-pip

# Arch Linux
sudo pacman -S python python-pip

# macOS
brew install python@3.12
```

### 依赖安装策略

脚本使用基于 hash 比对的智能安装策略：

```
检查 .venv/.deps_installed 标记文件
    ↓
标记文件存在？
    ├── 存在 → 读取标记中存储的 pyproject.toml 的 MD5 hash
    │         与当前 pyproject.toml 的 MD5 hash 比对
    │         ├── 一致 → 跳过安装（节省时间）
    │         └── 不一致 → 执行 pip install 并更新标记
    └── 不存在 → 执行 pip install 并写入标记
```

**优点：**

- 首次运行自动安装所有依赖
- 后续启动秒级完成（跳过安装）
- 修改 `pyproject.toml` 后自动触发依赖更新
- `reset` 和 `install` 命令强制重装

### 脚本库依赖

脚本复用项目现有的 Bash 脚本库：

| 库文件 | 路径 | 使用的函数 |
|--------|------|-----------|
| `lib_common.sh` | `scripts/lib/bash/lib_common.sh` | `log_info`, `log_success`, `log_warn`, `log_error`, `log_cyan` |

### 各命令详细说明

#### `serve` — 开发服务器

启动 MkDocs 内置的开发服务器，支持文件修改后自动热重载。

```bash
./scripts/document/mkdocs_dev.sh serve -p 3000 -b 0.0.0.0
# 访问 http://0.0.0.0:3000 预览文档
```

**行为：**

1. 检查 Python 版本
2. 确保 `.venv` 存在并激活
3. 检查并安装依赖
4. 执行 `mkdocs serve --dev-addr=ADDR:PORT`

**输出示例：**

```
=== MkDocs 开发服务器 ===
地址: http://127.0.0.1:8000
文档: /home/user/CFDesktop/document

INFO    -  Building documentation...
INFO    -  Cleaning site directory
INFO    -  Documentation built in 2.35 seconds
INFO    -  [12:00:00] Watching paths for changes
```

#### `build` — 构建静态站点

构建完整的静态网站，输出到 `out/docs/site/` 目录。

```bash
./scripts/document/mkdocs_dev.sh build
# 输出到 out/docs/site/
```

**行为：**

1. 确保环境就绪
2. 执行 `mkdocs build --clean -d out/docs/site`
3. `--clean` 参数确保每次构建都是全新输出

**构建产物结构：**

```
out/docs/site/
├── index.html              # 首页
├── assets/                 # 静态资源（CSS/JS/字体）
├── search/                 # 搜索索引
├── HandBook/               # 手册页面
├── development/            # 开发文档页面
├── design_stage/           # 设计文档页面
└── ...                     # 其他文档页面
```

#### `api` — API 文档管线

运行完整的 Doxygen → Doxybook2 → Markdown 管线，从 C++ 头文件生成 API 参考文档。

```bash
./scripts/document/mkdocs_dev.sh api
```

**管线流程：**

```
C++ 头文件 (*.h, *.hpp)
    ↓ doxygen Doxyfile
XML 格式输出 → xml/
    ↓ 清理旧文档
删除 document/api/
    ↓ doxybook2
Markdown 格式 → document/api/
    ↓ 完成
可通过 mkdocs serve 预览
```

**前置条件：**

```bash
# 安装 Doxygen
sudo apt install doxygen

# 安装 Doxybook2（从 GitHub Releases 下载）
wget https://github.com/matusnovak/doxybook2/releases/download/v1.5.0/doxybook2-linux-amd64-v1.5.0.zip
unzip doxybook2-linux-amd64-v1.5.0.zip
sudo mv bin/doxybook2 /usr/local/bin/
```

**使用的配置文件：**

| 文件 | 用途 |
|------|------|
| `Doxyfile` | Doxygen 配置（输入源码目录、输出格式等） |
| `doxybook.json` | Doxybook2 配置（输出目录结构、Markdown 格式等） |

#### `install` — 安装依赖

仅创建/更新虚拟环境，不启动任何服务。

```bash
./scripts/document/mkdocs_dev.sh install
./scripts/document/mkdocs_dev.sh install --verbose  # 显示详细安装过程
```

**适用场景：**

- CI/CD 环境初始化
- 首次克隆项目后设置文档开发环境
- 依赖更新后刷新环境

#### `clean` — 清理构建产物

清理所有文档构建产物，但保留虚拟环境（`.venv`）。

```bash
./scripts/document/mkdocs_dev.sh clean
```

**清理范围：**

| 清理目标 | 路径 | 说明 |
|----------|------|------|
| MkDocs 输出 | `out/docs/site/` | 静态站点构建产物 |
| Doxygen XML | `xml/` | Doxygen 生成的 XML 中间文件 |
| Python 缓存 | `__pycache__/` | Python 字节码缓存（递归清理） |

**注意：** `.venv/` 虚拟环境不会被删除，如需完全重置请使用 `reset` 命令。

#### `reset` — 重置虚拟环境

完全删除并重建 Python 虚拟环境。

```bash
./scripts/document/mkdocs_dev.sh reset
```

**行为：**

1. 删除 `.venv/` 目录
2. 重新创建虚拟环境
3. 安装所有依赖

**适用场景：**

- Python 依赖出现兼容性问题
- 虚拟环境损坏
- 需要确保干净的环境状态

### 返回码

| 返回码 | 说明 |
|--------|------|
| 0 | 成功 |
| 1 | Python 未安装 / 版本过低 / 虚拟环境创建失败 / 外部工具缺失 |

### 彩色日志输出

脚本使用 `lib_common.sh` 提供的统一日志函数，所有输出均带时间戳和颜色：

```
[2026-04-06 12:00:00] [INFO]    创建 Python 虚拟环境: /path/to/.venv      （青色）
[2026-04-06 12:00:00] [SUCCESS] 虚拟环境创建成功                             （绿色）
[2026-04-06 12:00:00] [WARNING] 依赖版本不匹配                               （黄色）
[2026-04-06 12:00:00] [ERROR]   Python 3 未安装                              （红色）
```

### 注意事项

1. **Python 版本** — 要求 Python >= 3.10，脚本启动时会自动检测
2. **虚拟环境位置** — `.venv/` 创建在项目根目录，已添加到 `.gitignore`
3. **幂等性** — 多次运行 `serve` 或 `install` 不会重复安装依赖
4. **依赖变更检测** — 修改 `pyproject.toml` 后自动触发依赖更新
5. **`api` 命令** — 需要额外安装 `doxygen` 和 `doxybook2`，脚本会检查并给出安装提示
6. **工作目录** — 脚本通过 `SCRIPT_DIR` 自动定位项目根目录，可在任意目录执行
7. **`set -eo pipefail`** — 启用严格错误处理，任何命令失败都会立即终止

### 完整输出示例

```
$ ./scripts/document/mkdocs_dev.sh serve

[2026-04-06 12:00:00] [INFO] 创建 Python 虚拟环境: /home/user/CFDesktop/.venv
[2026-04-06 12:00:03] [SUCCESS] 虚拟环境创建成功
[2026-04-06 12:00:03] [INFO] 安装文档开发依赖...
[2026-04-06 12:00:20] [SUCCESS] 依赖安装完成

=== MkDocs 开发服务器 ===
地址: http://127.0.0.1:8000
文档: /home/user/CFDesktop/document

INFO    -  Building documentation...
INFO    -  Documentation built in 2.35 seconds
INFO    -  [12:00:20] Serving on http://127.0.0.1:8000
INFO    -  [12:00:20] Watching paths for changes
```
