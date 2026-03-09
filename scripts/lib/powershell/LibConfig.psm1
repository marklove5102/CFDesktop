# =============================================================================
# CFDesktop Scripts Library - Config Module (PowerShell)
# =============================================================================
#
# 提供 INI 配置文件解析功能
#
# 用法:
#   . "$PSScriptRoot\LibConfig.ps1"
#   $config = Get-IniConfig -FilePath "config.ini"
#   $generator = $config["cmake"]["generator"]
#
# =============================================================================

function Get-IniConfig {
    <#
    .SYNOPSIS
        读取 INI 配置文件

    .PARAMETER FilePath
        INI 文件路径

    .OUTPUTS
        嵌套哈希表，结构为 @{ section = @{ key = value } }

    .EXAMPLE
        $config = Get-IniConfig -FilePath "build_config.ini"
        $buildType = $config["cmake"]["build_type"]
    #>
    param(
        [Parameter(Mandatory=$true)]
        [string]$FilePath
    )

    $config = @{}
    $currentSection = ""

    if (Test-Path $FilePath) {
        Get-Content $FilePath | ForEach-Object {
            $line = $_.Trim()

            # 跳过空行和注释
            if ([string]::IsNullOrEmpty($line) -or $line.StartsWith("#") -or $line.StartsWith(";")) {
                return
            }

            # Section 头部 [section_name]
            if ($line -match '^\[([^\]]+)\]$') {
                $currentSection = $matches[1]
                if (-not $config.ContainsKey($currentSection)) {
                    $config[$currentSection] = @{}
                }
                return
            }

            # Key=Value 对
            if ($line -match '^([^=]+)=(.*)$') {
                $key = $matches[1].Trim()
                $value = $matches[2].Trim()

                # 去除值两端的引号（如果有）
                if ($value -match '^"(.*)"$' -or $value -match "^'(.*)'$") {
                    $value = $matches[1]
                }

                if ($currentSection -and $config.ContainsKey($currentSection)) {
                    $config[$currentSection][$key] = $value
                }
            }
        }
        return $config
    }
    else {
        throw "Configuration file not found: $FilePath"
    }
}

function Get-IniValue {
    <#
    .SYNOPSIS
        获取特定的配置值

    .PARAMETER FilePath
        INI 文件路径

    .PARAMETER Section
        Section 名称

    .PARAMETER Key
        Key 名称

    .OUTPUTS
        配置值，如果未找到则返回空字符串

    .EXAMPLE
        $generator = Get-IniValue -FilePath "config.ini" -Section "cmake" -Key "generator"
    #>
    param(
        [Parameter(Mandatory=$true)]
        [string]$FilePath,
        [Parameter(Mandatory=$true)]
        [string]$Section,
        [Parameter(Mandatory=$true)]
        [string]$Key
    )

    $config = Get-IniConfig -FilePath $FilePath

    if ($config.ContainsKey($Section) -and $config[$Section].ContainsKey($Key)) {
        return $config[$Section][$Key]
    }

    return ""
}

function Test-IniValue {
    <#
    .SYNOPSIS
        检查配置项是否存在

    .PARAMETER FilePath
        INI 文件路径

    .PARAMETER Section
        Section 名称

    .PARAMETER Key
        Key 名称

    .OUTPUTS
        Boolean: True 表示存在，False 表示不存在

    .EXAMPLE
        if (Test-IniValue -FilePath "config.ini" -Section "cmake" -Key "generator") { ... }
    #>
    param(
        [Parameter(Mandatory=$true)]
        [string]$FilePath,
        [Parameter(Mandatory=$true)]
        [string]$Section,
        [Parameter(Mandatory=$true)]
        [string]$Key
    )

    $value = Get-IniValue -FilePath $FilePath -Section $Section -Key $Key
    return -not [string]::IsNullOrEmpty($value)
}

function Get-DefaultConfigFile {
    <#
    .SYNOPSIS
        获取默认配置文件路径

    .PARAMETER Mode
        配置模式: develop, deploy, ci

    .PARAMETER ScriptDir
        脚本目录（如果未指定，自动检测）

    .OUTPUTS
        配置文件的绝对路径

    .EXAMPLE
        $configFile = Get-DefaultConfigFile -Mode "develop"
    #>
    param(
        [Parameter(Mandatory=$false)]
        [ValidateSet("develop", "deploy", "ci")]
        [string]$Mode = "develop",
        [string]$ScriptDir = (Split-Path -Parent $MyInvocation.MyCommand.Path)
    )

    switch ($Mode) {
        "deploy" {
            return Join-Path $ScriptDir "build_deploy_config.ini"
        }
        "ci" {
            return Join-Path $ScriptDir "build_ci_config.ini"
        }
        default {
            return Join-Path $ScriptDir "build_develop_config.ini"
        }
    }
}

# 导出函数
Export-ModuleMember -Function `
    Get-IniConfig, `
    Get-IniValue, `
    Test-IniValue, `
    Get-DefaultConfigFile
