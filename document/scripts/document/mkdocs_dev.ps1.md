# mkdocs_dev.ps1

> 文档编写日期: 2026-04-06

## 使用办法 (Usage)

### 基本语法

```powershell
.\scripts\document\mkdocs_dev.ps1 <Command> [OPTIONS]
```

### 命令说明

| 命令 | 说明 |
|------|------|
| `serve` | 启动 MkDocs 开发服务器（默认命令） |
| `build` | 构建静态站点到 `out\docs\site\` |
| `api` | 运行 Doxygen + Doxybook2 API 文档管线 |
| `install` | 仅创建/更新虚拟环境和安装依赖 |
| `clean` | 清理构建产物（`out\docs\`、`__pycache__`、Doxygen XML） |
| `reset` | 删除并重建 `.venv` 虚拟环境 |
| `help` | 显示帮助信息 |

### 参数说明

| 参数 | 类型 | 说明 |
|------|------|------|
| `-Command` | string | 要执行的命令（默认: `serve`） |
| `-Port` | int | 开发服务器端口（默认: `8000`） |
| `-Bind` | string | 开发服务器绑定地址（默认: `127.0.0.1`） |
| `-VerboseFlag` | switch | 启用详细输出模式 |

### 使用示例

```powershell
# 启动开发服务器（默认 8000 端口）
.\scripts\document\mkdocs_dev.ps1 serve

# 自定义端口和绑定地址启动
.\scripts\document\mkdocs_dev.ps1 serve -Port 3000 -Bind "0.0.0.0"

# 构建静态站点
.\scripts\document\mkdocs_dev.ps1 build

# 生成 API 文档（需要 doxygen + doxybook2）
.\scripts\document\mkdocs_dev.ps1 api

# 仅安装依赖（不启动服务器）
.\scripts\document\mkdocs_dev.ps1 install

# 清理构建产物
.\scripts\document\mkdocs_dev.ps1 clean

# 完全重置虚拟环境
.\scripts\document\mkdocs_dev.ps1 reset

# 使用详细模式调试
.\scripts\document\mkdocs_dev.ps1 install -VerboseFlag

# 查看帮助
.\scripts\document\mkdocs_dev.ps1 help
```

## Scripts详解 (Detailed Explanation)

### 脚本用途

`mkdocs_dev.ps1` 是 `mkdocs_dev.sh` 的 Windows PowerShell 等效实现，提供完全一致的 MkDocs 文档开发环境管理功能。两者功能、命令和参数完全对等，确保跨平台开发体验一致。

### 与 Bash 版本的差异

| 方面 | `mkdocs_dev.sh` | `mkdocs_dev.ps1` |
|------|------------------|-------------------|
| 平台 | Linux / macOS | Windows |
| 虚拟环境激活 | `source .venv/bin/activate` | `. .venv\Scripts\Activate.ps1` |
| 参数风格 | `-p`, `--port` | `-Port` |
| 详细模式 | `-v`, `--verbose` | `-VerboseFlag` (switch) |
| 脚本库 | `lib_common.sh` | `LibCommon.psm1` |
| 路径分隔符 | `/` | `\` |
| 依赖 hash | `md5sum` | `Get-FileHash -Algorithm MD5` |

### 脚本常量

| 常量 | 值 | 说明 |
|------|-----|------|
| `$VenvDir` | `.venv` | 虚拟环境目录名（位于项目根目录） |
| `$DepsMarker` | `.deps_installed` | 依赖安装标记文件（位于 `.venv\` 内） |
| `$DocsOutputDir` | `out\docs\site` | 静态站点输出路径 |
| `$DoxygenXmlDir` | `xml` | Doxygen XML 输出目录 |
| `$DoxygenApiDir` | `document\api` | API 文档 Markdown 输出目录 |
| `$ProjectRoot` | 自动推导 | 项目根目录绝对路径 |
| `$ScriptDir` | `$PSScriptRoot` | 脚本所在目录 |

### 环境要求

| 依赖项 | 版本要求 | 用途 |
|--------|----------|------|
| **Python** | >= 3.10 | 运行 MkDocs 及相关工具 |
| **PowerShell** | >= 5.1 | 脚本运行环境 |
| **pip** | 随 Python 安装 | 包管理 |
| **doxygen** | 任意稳定版 | API 文档生成（仅 `api` 命令需要） |
| **doxybook2** | >= 1.5.0 | Doxygen XML → Markdown 转换（仅 `api` 命令需要） |

### Python 安装指引

```powershell
# 方法 1: 官方安装器
# 从 https://www.python.org/downloads/ 下载安装
# 安装时勾选 "Add Python to PATH"

# 方法 2: winget
winget install Python.Python.3.12

# 方法 3: Chocolatey
choco install python

# 方法 4: Scoop
scoop install python
```

### 模块导入

脚本从 `scripts/lib/powershell/` 目录导入以下模块：

```powershell
Import-Module LibCommon.psm1  # 提供日志函数 (Write-LogInfo, Write-LogSuccess 等)
```

### 依赖安装策略

与 Bash 版本一致的智能安装策略：

```
检查 .venv\.deps_installed 标记文件
    ↓
标记文件存在？
    ├── 存在 → 读取标记中存储的 pyproject.toml 的 MD5 hash
    │         与当前 pyproject.toml 的 MD5 hash 比对（Get-FileHash）
    │         ├── 一致 → 跳过安装
    │         └── 不一致 → 执行 pip install 并更新标记
    └── 不存在 → 执行 pip install 并写入标记
```

### 各命令详细说明

#### `serve` — 开发服务器

```powershell
.\scripts\document\mkdocs_dev.ps1 serve -Port 3000 -Bind "0.0.0.0"
```

**行为：**

1. 调用 `Test-Python` 检查 Python 环境
2. 调用 `New-Venv` 确保虚拟环境存在
3. 调用 `Enable-Venv` 激活虚拟环境
4. 调用 `Install-DocDeps` 确保依赖已安装
5. 使用 `Push-Location` 切换到项目根目录
6. 执行 `mkdocs serve --dev-addr=ADDR:PORT`
7. 使用 `try/finally` 确保退出时恢复工作目录

#### `build` — 构建静态站点

```powershell
.\scripts\document\mkdocs_dev.ps1 build
```

**行为：**

1. 确保环境就绪（同 `serve`）
2. 执行 `mkdocs build --clean -d out\docs\site`
3. 输出构建完成信息

#### `api` — API 文档管线

```powershell
.\scripts\document\mkdocs_dev.ps1 api
```

**管线流程（与 Bash 版本一致）：**

```
C++ 头文件 (*.h, *.hpp) → doxygen Doxyfile → xml\ → doxybook2 → document\api\
```

**前置条件：**

```powershell
# 安装 Doxygen
# 从 https://www.doxygen.nl/download.html 下载 Windows 安装包
# 或使用 Chocolatey: choco install doxygen

# 安装 Doxybook2
# 从 https://github.com/matusnovak/doxybook2/releases 下载
```

#### `install` — 安装依赖

```powershell
.\scripts\document\mkdocs_dev.ps1 install
.\scripts\document\mkdocs_dev.ps1 install -VerboseFlag  # 详细输出
```

#### `clean` — 清理构建产物

```powershell
.\scripts\document\mkdocs_dev.ps1 clean
```

**清理范围：**

| 清理目标 | 路径 | 说明 |
|----------|------|------|
| MkDocs 输出 | `out\docs\site\` | 静态站点构建产物 |
| Doxygen XML | `xml\` | Doxygen 生成的 XML 中间文件 |
| Python 缓存 | `__pycache__\` | Python 字节码缓存（递归清理） |

#### `reset` — 重置虚拟环境

```powershell
.\scripts\document\mkdocs_dev.ps1 reset
```

完全删除 `.venv\` 并重新创建，安装所有依赖。

### 核心函数

| 函数名 | 说明 |
|--------|------|
| `Test-Python` | 检查 Python 是否安装（支持 `python` 和 `python3` 两种命令名） |
| `New-Venv` | 创建 Python 虚拟环境（如果不存在） |
| `Enable-Venv` | 激活虚拟环境（`.venv\Scripts\Activate.ps1`） |
| `Install-DocDeps` | 安装/更新依赖（带 hash 比对） |
| `Initialize-DocEnv` | 组合调用上述四个函数，确保环境就绪 |
| `Invoke-Serve` | 执行 `serve` 命令 |
| `Invoke-Build` | 执行 `build` 命令 |
| `Invoke-Api` | 执行 `api` 命令 |
| `Invoke-Install` | 执行 `install` 命令 |
| `Invoke-Clean` | 执行 `clean` 命令 |
| `Invoke-Reset` | 执行 `reset` 命令 |

### 返回码

| 返回码 | 说明 |
|--------|------|
| 0 | 成功 |
| 1 | Python 未安装 / 虚拟环境创建失败 / 外部工具缺失 |

### 彩色日志输出

使用 `LibCommon.psm1` 模块提供的统一日志函数：

```
[2026-04-06 12:00:00] [INFO]    创建 Python 虚拟环境: C:\CFDesktop\.venv       （青色）
[2026-04-06 12:00:00] [SUCCESS] 虚拟环境创建成功                                  （绿色）
[2026-04-06 12:00:00] [WARNING] 依赖版本不匹配                                    （黄色）
[2026-04-06 12:00:00] [ERROR]   Python 3 未安装                                   （红色）
```

### 注意事项

1. **执行策略** — Windows 可能需要设置 PowerShell 执行策略：`Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser`
2. **Python PATH** — 确保 Python 已添加到系统 PATH（安装时勾选 "Add Python to PATH"）
3. **`$ErrorActionPreference = "Stop"`** — 启用严格错误处理，任何命令失败都会立即终止
4. **`Push-Location` / `Pop-Location`** — 使用 `try/finally` 模式确保工作目录正确恢复
5. **Python 命令兼容** — 同时检测 `python` 和 `python3` 命令
6. **与 Bash 版本完全对等** — 命令、参数、行为一致，确保跨平台体验统一
7. **虚拟环境位置** — `.venv\` 创建在项目根目录，已添加到 `.gitignore`
8. **幂等性** — 多次运行不会重复安装依赖

### 完整输出示例

```
PS C:\CFDesktop> .\scripts\document\mkdocs_dev.ps1 serve

[2026-04-06 12:00:00] [INFO] 创建 Python 虚拟环境: C:\CFDesktop\.venv
[2026-04-06 12:00:03] [SUCCESS] 虚拟环境创建成功
[2026-04-06 12:00:03] [INFO] 安装文档开发依赖...
[2026-04-06 12:00:20] [SUCCESS] 依赖安装完成

=== MkDocs 开发服务器 ===
地址: http://127.0.0.1:8000
文档: C:\CFDesktop\document

INFO    -  Building documentation...
INFO    -  Documentation built in 2.35 seconds
INFO    -  [12:00:20] Serving on http://127.0.0.1:8000
INFO    -  [12:00:20] Watching paths for changes
```
