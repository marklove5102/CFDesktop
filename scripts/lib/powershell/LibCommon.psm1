# =============================================================================
# CFDesktop Scripts Library - Common Module (PowerShell)
# =============================================================================
#
# 提供统一的日志输出功能
#
# 用法:
#   . "$PSScriptRoot\LibCommon.ps1"
#   Write-LogInfo "This is an info message"
#   Write-LogError "This is an error message"
#
# =============================================================================

function Write-Log {
    <#
    .SYNOPSIS
        写入带时间戳和颜色编码的日志消息

    .PARAMETER Message
        要输出的消息内容

    .PARAMETER Level
        日志级别: INFO, SUCCESS, WARNING, ERROR (默认: INFO)

    .EXAMPLE
        Write-Log -Message "Build completed" -Level "SUCCESS"
    #>
    param(
        [Parameter(Mandatory=$true)]
        [string]$Message,
        [Parameter(Mandatory=$false)]
        [ValidateSet("INFO", "SUCCESS", "WARNING", "ERROR")]
        [string]$Level = "INFO"
    )

    $timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    $color = switch ($Level) {
        "INFO"    { "Cyan" }
        "SUCCESS" { "Green" }
        "WARNING" { "Yellow" }
        "ERROR"   { "Red" }
        default   { "White" }
    }

    Write-Host "[$timestamp] [$Level] $Message" -ForegroundColor $color
}

function Write-LogInfo {
    <#
    .SYNOPSIS
        写入 INFO 级别日志

    .EXAMPLE
        Write-LogInfo "Starting build process"
    #>
    param(
        [Parameter(Mandatory=$true, ValueFromRemainingArguments=$true)]
        [string[]]$Message
    )
    Write-Log -Message ($Message -join ' ') -Level "INFO"
}

function Write-LogSuccess {
    <#
    .SYNOPSIS
        写入 SUCCESS 级别日志

    .EXAMPLE
        Write-LogSuccess "Build completed successfully"
    #>
    param(
        [Parameter(Mandatory=$true, ValueFromRemainingArguments=$true)]
        [string[]]$Message
    )
    Write-Log -Message ($Message -join ' ') -Level "SUCCESS"
}

function Write-LogWarning {
    <#
    .SYNOPSIS
        写入 WARNING 级别日志

    .EXAMPLE
        Write-LogWarning "Configuration file not found, using defaults"
    #>
    param(
        [Parameter(Mandatory=$true, ValueFromRemainingArguments=$true)]
        [string[]]$Message
    )
    Write-Log -Message ($Message -join ' ') -Level "WARNING"
}

function Write-LogError {
    <#
    .SYNOPSIS
        写入 ERROR 级别日志

    .EXAMPLE
        Write-LogError "Build failed with exit code: 1"
    #>
    param(
        [Parameter(Mandatory=$true, ValueFromRemainingArguments=$true)]
        [string[]]$Message
    )
    Write-Log -Message ($Message -join ' ') -Level "ERROR"
}

function Write-LogSeparator {
    <#
    .SYNOPSIS
        写入分隔线

    .PARAMETER Char
        分隔线字符 (默认: "=")

    .PARAMETER Width
        分隔线宽度 (默认: 40)

    .EXAMPLE
        Write-LogSeparator
        Write-LogSeparator -Char "-" -Width 60
    #>
    param(
        [string]$Char = "=",
        [int]$Width = 40
    )
    $line = $Char * $Width
    Write-LogInfo $line
}

function Write-LogProgress {
    <#
    .SYNOPSIS
        显示进度信息

    .PARAMETER Current
        当前进度

    .PARAMETER Total
        总数

    .PARAMETER Message
        进度描述

    .EXAMPLE
        Write-LogProgress -Current 5 -Total 10 -Message "Processing files"
    #>
    param(
        [int]$Current,
        [int]$Total,
        [string]$Message = "Processing"
    )
    $percent = [math]::Round(($Current / $Total) * 100)
    Write-LogInfo "[$Current/$Total] ($percent%) $Message"
}

# 导出函数
Export-ModuleMember -Function `
    Write-Log, `
    Write-LogInfo, `
    Write-LogSuccess, `
    Write-LogWarning, `
    Write-LogError, `
    Write-LogSeparator, `
    Write-LogProgress
