# =============================================================================
# CFDesktop Scripts Library - Arguments Module (PowerShell)
# =============================================================================
#
# 提供参数解析辅助函数
#
# 用法:
#   . "$PSScriptRoot\LibArgs.ps1"
#   $mode = Parse-ConfigMode $args[0]
#
# =============================================================================

function Parse-ConfigMode {
    <#
    .SYNOPSIS
        解析配置模式参数

    .PARAMETER Mode
        模式字符串

    .OUTPUTS
        如果有效则返回模式字符串，否则返回 null

    .EXAMPLE
        $mode = Parse-ConfigMode "develop"
    #>
    param(
        [Parameter(Mandatory=$true)]
        [string]$Mode
    )

    switch ($Mode) {
        { $_ -in @("develop", "deploy", "ci") } {
            return $Mode
        }
        default {
            return $null
        }
    }
}

function Test-ValidConfigMode {
    <#
    .SYNOPSIS
        测试是否为有效的配置模式

    .PARAMETER Mode
        模式字符串

    .OUTPUTS
        Boolean: True 表示有效，False 表示无效

    .EXAMPLE
        if (Test-ValidConfigMode "develop") { ... }
    #>
    param(
        [Parameter(Mandatory=$true)]
        [string]$Mode
    )
    $null -ne (Parse-ConfigMode -Mode $Mode)
}

function Show-StandardUsage {
    <#
    .SYNOPSIS
        显示标准用法帮助

    .PARAMETER ScriptName
        脚本名称（可选，默认为当前脚本名）

    .EXAMPLE
        Show-StandardUsage
    #>
    param(
        [string]$ScriptName = (Split-Path -Leaf $MyInvocation.ScriptName)
    )
    Write-Host "Usage: $ScriptName [develop|deploy|ci] [-c|--config <config_file>]"
}

function Show-DetailedUsage {
    <#
    .SYNOPSIS
        显示详细帮助信息

    .PARAMETER ScriptName
        脚本名称

    .PARAMETER Description
        额外说明（可选）

    .EXAMPLE
        Show-DetailedUsage -ScriptName "build.ps1" -Description "Build the project"
    #>
    param(
        [string]$ScriptName = (Split-Path -Leaf $MyInvocation.ScriptName),
        [string]$Description = ""
    )

    Write-Host ""
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host "  $ScriptName" -ForegroundColor Cyan
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host ""

    if (-not [string]::IsNullOrEmpty($Description)) {
        Write-Host $Description
        Write-Host ""
    }

    Write-Host "Arguments:"
    Write-Host "  develop    Use development build configuration"
    Write-Host "  deploy     Use deployment build configuration"
    Write-Host "  ci         Use CI build configuration"
    Write-Host ""

    Write-Host "Options:"
    Write-Host "  -c, --config <file>  Use specified configuration file"
    Write-Host "  -h, --help           Show this help message"
    Write-Host ""

    Write-Host "Examples:"
    Write-Host "  .\$ScriptName develop"
    Write-Host "  .\$ScriptName deploy -c custom_config.ini"
    Write-Host ""
}

function Test-HelpArg {
    <#
    .SYNOPSIS
        检查是否为帮助参数

    .PARAMETER Arg
        参数值

    .OUTPUTS
        Boolean: True 表示是帮助参数

    .EXAMPLE
        if (Test-HelpArg $args[0]) { Show-DetailedUsage; exit }
    #>
    param(
        [Parameter(Mandatory=$true)]
        [string]$Arg
    )

    return $Arg -in @("-h", "--help", "help")
}

function Show-UnknownArgError {
    <#
    .SYNOPSIS
        显示未知参数错误

    .PARAMETER Arg
        未知参数

    .PARAMETER ScriptName
        脚本名称（可选）

    .EXAMPLE
        Show-UnknownArgError -Arg "--unknown"
    #>
    param(
        [Parameter(Mandatory=$true)]
        [string]$Arg,
        [string]$ScriptName = (Split-Path -Leaf $MyInvocation.ScriptName)
    )

    Write-Host "ERROR: Unknown argument '$Arg'" -ForegroundColor Red
    Write-Host "Run '.\$ScriptName --help' for usage information." -ForegroundColor Yellow
}

function Show-MissingValueError {
    <#
    .SYNOPSIS
        显示缺失参数值错误

    .PARAMETER Arg
        参数名

    .EXAMPLE
        Show-MissingValueError -Arg "--config"
    #>
    param(
        [Parameter(Mandatory=$true)]
        [string]$Arg
    )

    Write-Host "ERROR: Missing value for argument '$Arg'" -ForegroundColor Red
}

# 导出函数
Export-ModuleMember -Function `
    Parse-ConfigMode, `
    Test-ValidConfigMode, `
    Show-StandardUsage, `
    Show-DetailedUsage, `
    Test-HelpArg, `
    Show-UnknownArgError, `
    Show-MissingValueError
