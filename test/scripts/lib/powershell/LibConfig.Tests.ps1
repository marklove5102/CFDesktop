# =============================================================================
# Unit Tests for LibConfig.psm1
# =============================================================================
#
# Usage:
#   Invoke-Pester LibConfig.Tests.ps1
#
# Requires: Pester 3.4+ or 5.x
#
# =============================================================================

# 加载被测试的模块
$LibDir = Join-Path (Split-Path (Split-Path (Split-Path (Split-Path $PSScriptRoot -Parent) -Parent) -Parent) -Parent) "scripts\lib\powershell"
Import-Module (Join-Path $LibDir "LibConfig.psm1") -Force

# 辅助函数：创建测试用的 INI 文件内容
function New-TestIniFile {
    param([string]$Path)

    $content = @'
# This is a comment
[cmake]
generator = Ninja
build_type = Release

[paths]
source = .
build_dir = build

[options]
use_ccache = true
parallel_jobs = 4
'@
    $content | Out-File -FilePath $Path -Encoding utf8
}

Describe "LibConfig" {
    It "Should read INI file and return config hashtable" {
        $TestIniPath = Join-Path $TestDrive "test_config_1.ini"
        New-TestIniFile -Path $TestIniPath

        $config = Get-IniConfig -FilePath $TestIniPath
        $config | Should Not BeNullOrEmpty
    }

    It "Should parse cmake section correctly" {
        $TestIniPath = Join-Path $TestDrive "test_config_2.ini"
        New-TestIniFile -Path $TestIniPath

        $config = Get-IniConfig -FilePath $TestIniPath
        $config.ContainsKey("cmake") | Should Be $true
        $config["cmake"]["generator"] | Should Be "Ninja"
        $config["cmake"]["build_type"] | Should Be "Release"
    }

    It "Should parse paths section correctly" {
        $TestIniPath = Join-Path $TestDrive "test_config_3.ini"
        New-TestIniFile -Path $TestIniPath

        $config = Get-IniConfig -FilePath $TestIniPath
        $config.ContainsKey("paths") | Should Be $true
        $config["paths"]["source"] | Should Be "."
        $config["paths"]["build_dir"] | Should Be "build"
    }

    It "Should parse options section correctly" {
        $TestIniPath = Join-Path $TestDrive "test_config_4.ini"
        New-TestIniFile -Path $TestIniPath

        $config = Get-IniConfig -FilePath $TestIniPath
        $config.ContainsKey("options") | Should Be $true
        $config["options"]["use_ccache"] | Should Be "true"
        $config["options"]["parallel_jobs"] | Should Be "4"
    }

    It "Should get specific value with Get-IniValue" {
        $TestIniPath = Join-Path $TestDrive "test_config_5.ini"
        New-TestIniFile -Path $TestIniPath

        $value = Get-IniValue -FilePath $TestIniPath -Section "cmake" -Key "generator"
        $value | Should Be "Ninja"
    }

    It "Should return empty string for missing key" {
        $TestIniPath = Join-Path $TestDrive "test_config_6.ini"
        New-TestIniFile -Path $TestIniPath

        $value = Get-IniValue -FilePath $TestIniPath -Section "cmake" -Key "nonexistent"
        $value | Should Be ""
    }

    It "Should test if value exists with Test-IniValue" {
        $TestIniPath = Join-Path $TestDrive "test_config_7.ini"
        New-TestIniFile -Path $TestIniPath

        $exists = Test-IniValue -FilePath $TestIniPath -Section "cmake" -Key "generator"
        $exists | Should Be $true

        $notExists = Test-IniValue -FilePath $TestIniPath -Section "cmake" -Key "nonexistent"
        $notExists | Should Be $false
    }

    It "Should throw error for non-existent file" {
        $threw = $false
        try {
            Get-IniConfig -FilePath "C:\nonexistent\file.ini" -ErrorAction Stop
        }
        catch {
            $threw = $true
        }
        $threw | Should Be $true
    }
}
