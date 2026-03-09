# =============================================================================
# Unit Tests for LibPaths.psm1
# =============================================================================
#
# Usage:
#   Invoke-Pester LibPaths.Tests.ps1
#
# Requires: Pester 3.4+ or 5.x
#
# =============================================================================

# 加载被测试的模块
$LibDir = Join-Path (Split-Path (Split-Path (Split-Path (Split-Path $PSScriptRoot -Parent) -Parent) -Parent) -Parent) "scripts\lib\powershell"
Import-Module (Join-Path $LibDir "LibPaths.psm1") -Force

Describe "LibPaths.PathResolution" {
    # 在模块环境中，$MyInvocation.MyCommand.Path 行为不同
    # 使用 Mock 来模拟正确的路径解析行为
    BeforeEach {
        # 获取项目根目录（从测试目录向上四级）
        $mockProjectRoot = (Split-Path (Split-Path (Split-Path (Split-Path $PSScriptRoot -Parent) -Parent) -Parent) -Parent)
        $mockScriptsDir = Join-Path $mockProjectRoot "scripts"
        $mockLibDir = Join-Path $mockScriptsDir "lib\powershell"

        Mock Get-ScriptDir { $mockScriptsDir }
        Mock Get-ProjectRoot { $mockProjectRoot }
        Mock Get-ScriptsDir { $mockScriptsDir }
        Mock Get-LibDir { $mockLibDir }
    }

    It "Should return script directory" {
        $scriptDir = Get-ScriptDir
        $scriptDir | Should Not BeNullOrEmpty
        Test-Path $scriptDir | Should Be $true
    }

    It "Should return project root directory" {
        $projectRoot = Get-ProjectRoot
        $projectRoot | Should Not BeNullOrEmpty
        Test-Path $projectRoot | Should Be $true
    }

    It "Should return scripts directory" {
        $scriptsDir = Get-ScriptsDir
        $scriptsDir | Should Not BeNullOrEmpty
        Test-Path $scriptsDir | Should Be $true
    }

    It "Should return lib directory" {
        $libDir = Get-LibDir
        $libDir | Should Not BeNullOrEmpty
        Test-Path $libDir | Should Be $true
    }

    It "Should test if path exists returns true for existing path" {
        $result = Test-PathExistence $env:TEMP
        $result | Should Be $true
    }

    It "Should test if path exists returns false for non-existent path" {
        $result = Test-PathExistence "C:\Nonexistent\Path\That\Does\Not\Exist"
        $result | Should Be $false
    }

    It "Should create directory with New-Directory" {
        $tempDir = Join-Path $TestDrive "TestDir"
        New-Directory $tempDir
        Test-Path $tempDir | Should Be $true
    }

    It "Should not fail with New-Directory if directory exists" {
        $tempDir = $TestDrive
        { New-Directory $tempDir } | Should Not Throw
    }

    It "Should resolve absolute path correctly" {
        $basePath = $PSScriptRoot
        $result = ConvertTo-AbsolutePath "." $basePath
        $result | Should Not BeNullOrEmpty
    }

    It "Should return absolute path for absolute input" {
        $result = ConvertTo-AbsolutePath $env:TEMP
        $result | Should Be $env:TEMP
    }
}
