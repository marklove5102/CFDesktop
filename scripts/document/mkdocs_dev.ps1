#!/usr/bin/env pwsh
<#
.SYNOPSIS
    MkDocs documentation development environment manager

.DESCRIPTION
    Manages Python virtual environment and MkDocs documentation development workflow.
    Supports serving, building, API documentation generation, and environment management.

.PARAMETER Command
    The command to execute:
    - serve:    Start MkDocs dev server (default)
    - build:    Build static site to out/docs/site/
    - api:      Run Doxygen + Doxybook2 API documentation pipeline
    - install:  Create/update virtual environment and install dependencies
    - clean:    Clean build artifacts (out/docs/, __pycache__, Doxygen XML)
    - reset:    Delete and recreate .venv
    - help:     Show this help message

.PARAMETER Port
    Dev server port (default: 8000)

.PARAMETER Bind
    Dev server bind address (default: 127.0.0.1)

.PARAMETER Verbose
    Enable verbose output

.EXAMPLE
    .\scripts\document\mkdocs_dev.ps1 serve
    Start MkDocs dev server on default port

.EXAMPLE
    .\scripts\document\mkdocs_dev.ps1 serve -Port 3000
    Start MkDocs dev server on port 3000

.EXAMPLE
    .\scripts\document\mkdocs_dev.ps1 build
    Build static site to out/docs/site/

.EXAMPLE
    .\scripts\document\mkdocs_dev.ps1 api
    Generate API documentation from C++ source

.EXAMPLE
    .\scripts\document\mkdocs_dev.ps1 install
    Create/update virtual environment

.EXAMPLE
    .\scripts\document\mkdocs_dev.ps1 reset
    Delete and recreate .venv
#>

param(
    [Parameter(Position=0)]
    [ValidateSet("serve", "build", "api", "install", "clean", "reset", "help")]
    [string]$Command = "serve",

    [Parameter()]
    [int]$Port = 8000,

    [Parameter()]
    [string]$Bind = "127.0.0.1",

    [Parameter()]
    [switch]$VerboseFlag
)

# Import library modules
$LibDir = Join-Path (Split-Path -Parent $PSScriptRoot) "lib\powershell"
Import-Module (Join-Path $LibDir "LibCommon.psm1") -Force

# Constants
$VenvDir = ".venv"
$DepsMarker = ".deps_installed"
$DocsOutputDir = "out\docs\site"
$DoxygenXmlDir = "xml"
$DoxygenApiDir = "document\api"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$ScriptDir = $PSScriptRoot

# =============================================================================
# Python environment
# =============================================================================

function Test-Python {
    $python = Get-Command "python" -ErrorAction SilentlyContinue
    if ($null -eq $python) {
        $python = Get-Command "python3" -ErrorAction SilentlyContinue
    }
    if ($null -eq $python) {
        Write-LogError "Python 3 未安装，请先安装 Python >= 3.10"
        Write-Host "  Windows: https://www.python.org/downloads/" -ForegroundColor Gray
        return $false
    }

    $version = & $python.Source --version 2>&1
    if ($VerboseFlag) { Write-LogInfo "[VERBOSE] Found: $version" }
    return $true
}

function New-Venv {
    $venvPath = Join-Path $ProjectRoot $VenvDir

    if (Test-Path $venvPath) {
        if ($VerboseFlag) { Write-LogInfo "[VERBOSE] 虚拟环境已存在: $venvPath" }
        return
    }

    Write-LogInfo "创建 Python 虚拟环境: $venvPath"
    $python = Get-Command "python" -ErrorAction SilentlyContinue
    if ($null -eq $python) {
        $python = Get-Command "python3" -ErrorAction SilentlyContinue
    }
    & $python.Source -m venv $venvPath

    if (-not (Test-Path $venvPath)) {
        Write-LogError "虚拟环境创建失败"
        exit 1
    }

    Write-LogSuccess "虚拟环境创建成功"
}

function Enable-Venv {
    $activateScript = Join-Path $ProjectRoot "$VenvDir\Scripts\Activate.ps1"

    if (-not (Test-Path $activateScript)) {
        Write-LogError "虚拟环境激活脚本不存在: $activateScript"
        Write-LogInfo "请先运行: .\scripts\document\mkdocs_dev.ps1 install"
        exit 1
    }

    . $activateScript
    if ($VerboseFlag) { Write-LogInfo "[VERBOSE] 虚拟环境已激活" }
}

function Install-DocDeps {
    $marker = Join-Path $ProjectRoot "$VenvDir\$DepsMarker"
    $pyproject = Join-Path $ScriptDir "pyproject.toml"

    # Check if deps are already installed and pyproject hasn't changed
    if (Test-Path $marker) {
        $markerHash = Get-Content $marker -ErrorAction SilentlyContinue
        $pyprojectHash = (Get-FileHash $pyproject -Algorithm MD5).Hash

        if ($markerHash -eq $pyprojectHash) {
            if ($VerboseFlag) { Write-LogInfo "[VERBOSE] 依赖已安装且未变更，跳过安装" }
            return
        }
    }

    Write-LogInfo "安装文档开发依赖..."
    pip install --upgrade pip --quiet
    pip install -e $ScriptDir --quiet

    # Write marker with pyproject hash for change detection
    (Get-FileHash $pyproject -Algorithm MD5).Hash | Set-Content $marker
    Write-LogSuccess "依赖安装完成"
}

function Initialize-DocEnv {
    if (-not (Test-Python)) { exit 1 }
    New-Venv
    Enable-Venv
    Install-DocDeps
}

# =============================================================================
# Command implementations
# =============================================================================

function Invoke-Serve {
    Initialize-DocEnv

    Write-Host ""
    Write-Host "=== MkDocs 开发服务器 ===" -ForegroundColor Cyan
    Write-LogInfo "地址: http://${Bind}:${Port}"
    Write-LogInfo "文档: $ProjectRoot\document"
    Write-Host ""

    Push-Location $ProjectRoot
    try {
        mkdocs serve --dev-addr="${Bind}:${Port}"
    } finally {
        Pop-Location
    }
}

function Invoke-Build {
    Initialize-DocEnv

    $outputPath = Join-Path $ProjectRoot $DocsOutputDir

    Write-Host ""
    Write-Host "=== MkDocs 构建 ===" -ForegroundColor Cyan
    Write-LogInfo "输出: $outputPath"
    Write-Host ""

    Push-Location $ProjectRoot
    try {
        mkdocs build --clean -d $DocsOutputDir
    } finally {
        Pop-Location
    }

    Write-LogSuccess "构建完成: $outputPath"
}

function Invoke-Api {
    # Check external tools
    $doxygen = Get-Command "doxygen" -ErrorAction SilentlyContinue
    if ($null -eq $doxygen) {
        Write-LogError "Doxygen 未安装"
        Write-Host "  Windows: https://www.doxygen.nl/download.html" -ForegroundColor Gray
        exit 1
    }

    $doxybook2 = Get-Command "doxybook2" -ErrorAction SilentlyContinue
    if ($null -eq $doxybook2) {
        Write-LogError "Doxybook2 未安装"
        Write-Host "  请从 https://github.com/matusnovak/doxybook2/releases 下载" -ForegroundColor Gray
        exit 1
    }

    Write-Host ""
    Write-Host "=== API 文档管线 ===" -ForegroundColor Cyan
    Write-Host ""

    Push-Location $ProjectRoot
    try {
        # Step 1: Run Doxygen
        Write-LogInfo "[1/3] 运行 Doxygen..."
        & doxygen Doxyfile

        # Step 2: Clean old API docs
        Write-LogInfo "[2/3] 清理旧 API 文档..."
        if (Test-Path $DoxygenApiDir) {
            Remove-Item -Recurse -Force $DoxygenApiDir
        }
        New-Item -ItemType Directory -Path $DoxygenApiDir -Force | Out-Null

        # Step 3: Run Doxybook2
        Write-LogInfo "[3/3] 运行 Doxybook2..."
        & doxybook2 --input ".\$DoxygenXmlDir" `
                     --output ".\$DoxygenApiDir" `
                     --config doxybook.json

        Write-LogSuccess "API 文档已生成到 $DoxygenApiDir"
    } finally {
        Pop-Location
    }
}

function Invoke-Install {
    Initialize-DocEnv
    Write-LogSuccess "环境准备就绪"
}

function Invoke-Clean {
    Write-Host ""
    Write-Host "=== 清理构建产物 ===" -ForegroundColor Cyan

    $cleaned = 0

    # Clean MkDocs output
    $docsPath = Join-Path $ProjectRoot $DocsOutputDir
    if (Test-Path $docsPath) {
        Remove-Item -Recurse -Force $docsPath
        Write-LogInfo "已清理: $DocsOutputDir"
        $cleaned++
    }

    # Clean Doxygen XML
    $xmlPath = Join-Path $ProjectRoot $DoxygenXmlDir
    if (Test-Path $xmlPath) {
        Remove-Item -Recurse -Force $xmlPath
        Write-LogInfo "已清理: $DoxygenXmlDir"
        $cleaned++
    }

    # Clean __pycache__
    $pycacheDirs = Get-ChildItem -Path $ProjectRoot -Directory -Recurse -Filter "__pycache__" -ErrorAction SilentlyContinue
    if ($pycacheDirs.Count -gt 0) {
        $pycacheDirs | Remove-Item -Recurse -Force
        Write-LogInfo "已清理: __pycache__ ($($pycacheDirs.Count) 个)"
        $cleaned++
    }

    if ($cleaned -eq 0) {
        Write-LogInfo "没有需要清理的内容"
    } else {
        Write-LogSuccess "清理完成 ($cleaned 项)"
    }
}

function Invoke-Reset {
    $venvPath = Join-Path $ProjectRoot $VenvDir

    Write-Host ""
    Write-Host "=== 重置虚拟环境 ===" -ForegroundColor Cyan

    if (Test-Path $venvPath) {
        Write-LogInfo "删除: $venvPath"
        Remove-Item -Recurse -Force $venvPath
        Write-LogSuccess "已删除旧环境"
    }

    # Recreate
    Initialize-DocEnv
    Write-LogSuccess "虚拟环境已重建"
}

# =============================================================================
# Execute command
# =============================================================================

$ErrorActionPreference = "Stop"

switch ($Command) {
    "serve"   { Invoke-Serve }
    "build"   { Invoke-Build }
    "api"     { Invoke-Api }
    "install" { Invoke-Install }
    "clean"   { Invoke-Clean }
    "reset"   { Invoke-Reset }
    "help"    { Get-Help $MyInvocation.MyCommand.Path -Detailed }
}
