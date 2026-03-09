# =============================================================================
# CFDesktop Scripts Library - Paths Module (PowerShell)
# =============================================================================
#
# 提供路径解析功能
#
# 用法:
#   . "$PSScriptRoot\LibPaths.ps1"
#   $scriptDir = Get-ScriptDir
#   $projectRoot = Get-ProjectRoot
#
# =============================================================================

function Get-ScriptDir {
    <#
    .SYNOPSIS
        获取调用脚本所在目录

    .OUTPUTS
        调用脚本所在目录的绝对路径

    .EXAMPLE
        $scriptDir = Get-ScriptDir
    #>
    # Method 0: Check global variables set by caller (most reliable for non-File execution)
    if ($global:CallerPSScriptRoot) {
        return $global:CallerPSScriptRoot
    }
    if ($global:CallerMyInvocationPath) {
        return Split-Path -Parent $global:CallerMyInvocationPath
    }

    # Method 1: Extract from command line (handles -File execution)
    try {
        $cmdLine = (Get-CimInstance Win32_Process -Filter "ProcessId = $PID").CommandLine
        if ($cmdLine -match '-File\s+"([^"]+\.ps1)"') {
            return Split-Path -Parent $matches[1]
        }
        elseif ($cmdLine -match '-File\s+(\S+\.ps1)') {
            return Split-Path -Parent $matches[1]
        }
    }
    catch {
        # Continue to next method
    }

    # Method 2: Use call stack - try ScriptPath first
    $callStack = Get-PSCallStack
    foreach ($frame in $callStack) {
        if ($frame.ScriptPath -and $frame.ScriptPath -notmatch '\.psm1$') {
            return Split-Path -Parent $frame.ScriptPath
        }
    }

    # Method 3: Try to infer from call stack Command property
    # This handles `.\script.ps1` and relative paths
    foreach ($frame in $callStack) {
        if ($frame.Command -match '\.ps1$' -and $frame.Command -ne '<ScriptBlock>') {
            $scriptPath = $frame.Command

            # If it's already an absolute path, use it directly
            if ([System.IO.Path]::IsPathRooted($scriptPath)) {
                if (Test-Path $scriptPath -PathType Leaf) {
                    return Split-Path -Parent $scriptPath
                }
            }

            # For relative paths, resolve against current directory
            $currentDir = (Get-Location).Path
            $resolvedPath = Join-Path $currentDir $scriptPath

            # Try the resolved path first
            if (Test-Path $resolvedPath -PathType Leaf) {
                return Split-Path -Parent $resolvedPath
            }

            # If not found, search in subdirectories (up to 3 levels deep)
            for ($depth = 1; $depth -le 3; $depth++) {
                $dirs = Get-ChildItem -Path $currentDir -Recurse -Directory -Depth $depth -ErrorAction SilentlyContinue |
                    Where-Object { $_.Name -eq 'scripts' }

                foreach ($dir in $dirs) {
                    $candidatePath = Join-Path $dir.FullName $scriptPath
                    if (Test-Path $candidatePath -PathType Leaf) {
                        return Split-Path -Parent $candidatePath
                    }
                }
            }

            # Search in parent directories
            $searchDir = $currentDir
            for ($i = 0; $i -lt 6; $i++) {
                $candidatePath = Join-Path $searchDir $scriptPath
                if (Test-Path $candidatePath -PathType Leaf) {
                    return Split-Path -Parent $candidatePath
                }
                $searchDir = Split-Path -Parent $searchDir
                if (-not $searchDir) { break }
            }
        }
    }

    # Method 4: Try MyInvocation from caller's scope
    try {
        $callerMyInvocation = Get-Variable -Scope 1 -Name MyInvocation -ErrorAction Stop
        if ($callerMyInvocation.Value.MyCommand.Path) {
            $path = $callerMyInvocation.Value.MyCommand.Path
            if ($path -notmatch '\.psm1$') {
                return Split-Path -Parent $path
            }
        }
    }
    catch {
        # Continue
    }

    # Method 5: Try PSScriptRoot from caller's scope
    try {
        $callerPSScriptRoot = Get-Variable -Scope 1 -Name PSScriptRoot -ErrorAction Stop
        if ($callerPSScriptRoot.Value) {
            $moduleDir = Join-Path $PSScriptRoot ""
            $callerDir = Join-Path $callerPSScriptRoot.Value ""
            if ($callerDir -ne $moduleDir) {
                return $callerPSScriptRoot.Value
            }
        }
    }
    catch {
        # Continue to fallback
    }

    # Last resort: use current location
    return (Get-Location).Path
}

function Get-ProjectRoot {
    <#
    .SYNOPSIS
        获取项目根目录
        假设脚本在 scripts/ 的子目录中

    .OUTPUTS
        项目根目录的绝对路径

    .EXAMPLE
        $projectRoot = Get-ProjectRoot
    #>
    $scriptDir = Get-ScriptDir
    Split-Path -Parent (Split-Path -Parent $scriptDir)
}

function Get-ScriptsDir {
    <#
    .SYNOPSIS
        获取 scripts 目录

    .OUTPUTS
        scripts 目录的绝对路径

    .EXAMPLE
        $scriptsDir = Get-ScriptsDir
    #>
    $projectRoot = Get-ProjectRoot
    Join-Path $projectRoot "scripts"
}

function Get-LibDir {
    <#
    .SYNOPSIS
        获取 lib 目录

    .OUTPUTS
        lib 目录的绝对路径

    .EXAMPLE
        $libDir = Get-LibDir
    #>
    $scriptsDir = Get-ScriptsDir
    Join-Path $scriptsDir "lib"
}

function Test-PathExistence {
    <#
    .SYNOPSIS
        检查路径是否存在

    .PARAMETER Path
        要检查的路径

    .OUTPUTS
        Boolean: True 表示存在，False 表示不存在

    .EXAMPLE
        if (Test-PathExistence "C:\Build") { ... }
    #>
    param(
        [Parameter(Mandatory=$true)]
        [string]$Path
    )
    Test-Path -Path $Path
}

function New-Directory {
    <#
    .SYNOPSIS
        确保目录存在，如果不存在则创建

    .PARAMETER Path
        要确保存在的目录路径

    .EXAMPLE
        New-Directory "C:\Build\Output"
    #>
    param(
        [Parameter(Mandatory=$true)]
        [string]$Path
    )
    if (!(Test-Path -Path $Path -PathType Container)) {
        New-Item -ItemType Directory -Path $Path -Force | Out-Null
    }
}

function ConvertTo-AbsolutePath {
    <#
    .SYNOPSIS
        解析相对路径为绝对路径

    .PARAMETER Path
        要解析的路径

    .PARAMETER BasePath
        基础路径（如果未指定，使用当前目录）

    .OUTPUTS
        解析后的绝对路径

    .EXAMPLE
        $absolutePath = ConvertTo-AbsolutePath "..\Build" (Get-ScriptDir)
    #>
    param(
        [Parameter(Mandatory=$true)]
        [string]$Path,
        [string]$BasePath = (Get-Location).Path
    )

    if ([System.IO.Path]::IsPathRooted($Path)) {
        return $Path
    }

    $resolved = Join-Path $BasePath $Path
    $resolved = (Resolve-Path -Path $resolved -ErrorAction SilentlyContinue).Path

    if ($resolved) {
        return $resolved
    }

    return Join-Path $BasePath $Path
}

# 导出函数
Export-ModuleMember -Function `
    Get-ScriptDir, `
    Get-ProjectRoot, `
    Get-ScriptsDir, `
    Get-LibDir, `
    Test-PathExistence, `
    New-Directory, `
    ConvertTo-AbsolutePath
