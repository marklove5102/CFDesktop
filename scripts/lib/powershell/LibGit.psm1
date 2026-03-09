# =============================================================================
# CFDesktop Scripts Library - Git Module (PowerShell)
# =============================================================================
#
# 提供 Git 相关的辅助函数，包括版本号解析和验证
#
# 用法:
#   . "$PSScriptRoot\LibGit.ps1"
#   $localVersion = Get-LocalVersion
#   $verifyLevel = Determine-VerifyLevel $localVersion $remoteVersion
#
# =============================================================================

function Get-MajorVersion {
    <#
    .SYNOPSIS
        解析版本号，返回主版本号

    .PARAMETER Version
        版本字符串（如 "1.2.3"）

    .OUTPUTS
        主版本号（如 "1"）

    .EXAMPLE
        $major = Get-MajorVersion -Version "1.2.3"
    #>
    param(
        [Parameter(Mandatory=$true)]
        [string]$Version
    )

    return ($Version -split '\.')[0]
}

function Get-MinorVersion {
    <#
    .SYNOPSIS
        解析版本号，返回次版本号

    .PARAMETER Version
        版本字符串（如 "1.2.3"）

    .OUTPUTS
        次版本号（如 "2"）

    .EXAMPLE
        $minor = Get-MinorVersion -Version "1.2.3"
    #>
    param(
        [Parameter(Mandatory=$true)]
        [string]$Version
    )

    $parts = $Version -split '\.'
    if ($parts.Length -ge 2) {
        return $parts[1]
    }
    return ""
}

function Get-PatchVersion {
    <#
    .SYNOPSIS
        解析版本号，返回补丁版本号

    .PARAMETER Version
        版本字符串（如 "1.2.3"）

    .OUTPUTS
        补丁版本号（如 "3"），如果不存在则返回空字符串

    .EXAMPLE
        $patch = Get-PatchVersion -Version "1.2.3"
    #>
    param(
        [Parameter(Mandatory=$true)]
        [string]$Version
    )

    $parts = $Version -split '\.'
    if ($parts.Length -ge 3) {
        return $parts[2]
    }
    return ""
}

function Compare-Versions {
    <#
    .SYNOPSIS
        比较两个版本号

    .PARAMETER Version1
        版本号 1

    .PARAMETER Version2
        版本号 2

    .OUTPUTS
        -1: Version1 < Version2
         0: Version1 = Version2
         1: Version1 > Version2

    .EXAMPLE
        $result = Compare-Versions -Version1 "1.2.3" -Version2 "1.2.4"
    #>
    param(
        [Parameter(Mandatory=$true)]
        [string]$Version1,
        [Parameter(Mandatory=$true)]
        [string]$Version2
    )

    $v1Parts = @($Version1 -split '\.' | ForEach-Object { [int]$_ })
    $v2Parts = @($Version2 -split '\.' | ForEach-Object { [int]$_ })

    # 填充较短的版本号
    $maxLength = [Math]::Max($v1Parts.Length, $v2Parts.Length)
    while ($v1Parts.Length -lt $maxLength) { $v1Parts += 0 }
    while ($v2Parts.Length -lt $maxLength) { $v2Parts += 0 }

    for ($i = 0; $i -lt $maxLength; $i++) {
        if ($v1Parts[$i] -lt $v2Parts[$i]) { return -1 }
        if ($v1Parts[$i] -gt $v2Parts[$i]) { return 1 }
    }

    return 0
}

function Get-LocalVersion {
    <#
    .SYNOPSIS
        获取当前分支的版本标签

    .OUTPUTS
        最近的版本标签，如果没有则返回 "0.0.0"

    .EXAMPLE
        $version = Get-LocalVersion
    #>
    try {
        $tag = git describe --tags --abbrev=0 2>$null
        if ($tag) {
            return $tag
        }
    }
    catch {
        # 忽略错误
    }
    return "0.0.0"
}

function Get-RemoteVersion {
    <#
    .SYNOPSIS
        获取远程 main 分支的版本标签

    .OUTPUTS
        远程 main 分支最近的版本标签，如果没有则返回 "0.0.0"

    .EXAMPLE
        $version = Get-RemoteVersion
    #>
    try {
        # 确保有最新的远程信息
        git fetch origin main:refs/remotes/origin/main --quiet 2>$null
        $tag = git describe --tags --abbrev=0 origin/main 2>$null
        if ($tag) {
            return $tag
        }
    }
    catch {
        # 忽略错误
    }
    return "0.0.0"
}

function Get-AllVersionTags {
    <#
    .SYNOPSIS
        获取所有的版本标签列表

    .OUTPUTS
        按版本排序的标签列表

    .EXAMPLE
        $tags = Get-AllVersionTags
    #>
    try {
        $tags = git tag -l --sort=-version:refname 2>$null
        if ($tags) {
            return $tags -split '\n'
        }
    }
    catch {
        # 忽略错误
    }
    return @()
}

function Get-LatestVersionTag {
    <#
    .SYNOPSIS
        获取最新的版本标签

    .OUTPUTS
        最新的版本标签

    .EXAMPLE
        $latest = Get-LatestVersionTag
    #>
    $tags = Get-AllVersionTags
    if ($tags.Count -gt 0) {
        return $tags[0]
    }
    return ""
}

function Get-CmakeVersion {
    <#
    .SYNOPSIS
        获取 CMakeLists.txt 中定义的版本号

    .PARAMETER ProjectRoot
        项目根目录路径

    .OUTPUTS
        CMakeLists.txt 中的版本号，如果未找到则返回空字符串

    .EXAMPLE
        $version = Get-CmakeVersion -ProjectRoot "C:\Project"
    #>
    param(
        [Parameter(Mandatory=$true)]
        [string]$ProjectRoot
    )

    $cmakeFile = Join-Path $ProjectRoot "CMakeLists.txt"

    if (Test-Path $cmakeFile) {
        $content = Get-Content $cmakeFile -Raw
        if ($content -match 'VERSION\s+([\d.]+)') {
            return $matches[1]
        }
    }

    return ""
}

function Determine-VerifyLevel {
    <#
    .SYNOPSIS
        根据本地版本和远程版本确定验证级别

    .PARAMETER LocalVersion
        本地版本号

    .PARAMETER RemoteVersion
        远程版本号

    .OUTPUTS
        major, minor, 或 patch

    .EXAMPLE
        $level = Determine-VerifyLevel -LocalVersion "1.2.3" -RemoteVersion "1.3.0"
    #>
    param(
        [string]$LocalVersion = "",
        [string]$RemoteVersion = ""
    )

    # 处理空版本号情况
    if ([string]::IsNullOrEmpty($LocalVersion)) {
        $LocalVersion = "0.0.0"
    }
    if ([string]::IsNullOrEmpty($RemoteVersion)) {
        $RemoteVersion = "0.0.0"
    }

    $localMajor = Get-MajorVersion -Version $LocalVersion
    $localMinor = Get-MinorVersion -Version $LocalVersion
    $remoteMajor = Get-MajorVersion -Version $RemoteVersion
    $remoteMinor = Get-MinorVersion -Version $RemoteVersion

    # 比较版本号
    if ($localMajor -ne $remoteMajor) {
        return "major"
    }
    elseif ($localMinor -ne $remoteMinor) {
        return "minor"
    }
    else {
        return "patch"
    }
}

function Get-VerifyLevelDescription {
    <#
    .SYNOPSIS
        获取验证级别的描述信息

    .PARAMETER Level
        验证级别 (major, minor, patch)

    .OUTPUTS
        验证级别的描述文本

    .EXAMPLE
        $desc = Get-VerifyLevelDescription -Level "major"
    #>
    param(
        [Parameter(Mandatory=$true)]
        [ValidateSet("major", "minor", "patch")]
        [string]$Level
    )

    switch ($Level) {
        "major" {
            return "Major 版本变更: X64 + ARM64 完整构建 + 测试"
        }
        "minor" {
            return "Minor 版本变更: X64 完整构建 + 测试"
        }
        "patch" {
            return "Patch 版本变更: X64 快速构建 + 测试"
        }
        default {
            return "未知验证级别: $Level"
        }
    }
}

function Test-UncommittedChanges {
    <#
    .SYNOPSIS
        检查是否有未提交的更改

    .OUTPUTS
        Boolean: True 表示有未提交的更改

    .EXAMPLE
        if (Test-UncommittedChanges) { Write-Warning "有未提交的更改" }
    #>
    try {
        $output = git diff-index --quiet HEAD -- 2>&1
        return $LASTEXITCODE -ne 0
    }
    catch {
        return $false
    }
}

function Get-CurrentBranch {
    <#
    .SYNOPSIS
        获取当前分支名称

    .OUTPUTS
        当前分支名称

    .EXAMPLE
        $branch = Get-CurrentBranch
    #>
    try {
        $branch = git rev-parse --abbrev-ref HEAD 2>$null
        if ($branch) {
            return $branch
        }
    }
    catch {
        # 忽略错误
    }
    return "unknown"
}

function Test-MainBranch {
    <#
    .SYNOPSIS
        检查是否在 main 分支上

    .OUTPUTS
        Boolean: True 表示在 main 分支

    .EXAMPLE
        if (Test-MainBranch) { Write-Host "在 main 分支上" }
    #>
    return (Get-CurrentBranch) -eq "main"
}

function Show-VersionInfo {
    <#
    .SYNOPSIS
        打印版本信息（用于调试）

    .PARAMETER LocalVersion
        本地版本

    .PARAMETER RemoteVersion
        远程版本

    .PARAMETER VerifyLevel
        验证级别（可选）

    .EXAMPLE
        Show-VersionInfo -LocalVersion "1.2.3" -RemoteVersion "1.3.0"
    #>
    param(
        [Parameter(Mandatory=$true)]
        [string]$LocalVersion,
        [Parameter(Mandatory=$true)]
        [string]$RemoteVersion,
        [string]$VerifyLevel = ""
    )

    Write-Host "版本信息:"
    Write-Host "  本地版本: $LocalVersion"
    Write-Host "  远程版本: $RemoteVersion"

    if (-not [string]::IsNullOrEmpty($VerifyLevel)) {
        Write-Host "  验证级别: $VerifyLevel"
        Write-Host "  $(Get-VerifyLevelDescription -Level $VerifyLevel)"
    }
}

# 导出函数
Export-ModuleMember -Function `
    Get-MajorVersion, `
    Get-MinorVersion, `
    Get-PatchVersion, `
    Compare-Versions, `
    Get-LocalVersion, `
    Get-RemoteVersion, `
    Get-AllVersionTags, `
    Get-LatestVersionTag, `
    Get-CmakeVersion, `
    Determine-VerifyLevel, `
    Get-VerifyLevelDescription, `
    Test-UncommittedChanges, `
    Get-CurrentBranch, `
    Test-MainBranch, `
    Show-VersionInfo
