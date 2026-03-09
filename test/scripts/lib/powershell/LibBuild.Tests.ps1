# =============================================================================
# Unit Tests for LibBuild.psm1
# =============================================================================
#
# Usage:
#   Invoke-Pester LibBuild.Tests.ps1
#
# Requires: Pester 3.4+ or 5.x
#
# =============================================================================

# 加载被测试的模块
$LibDir = Join-Path (Split-Path (Split-Path (Split-Path (Split-Path $PSScriptRoot -Parent) -Parent) -Parent) -Parent) "scripts\lib\powershell"
Import-Module (Join-Path $LibDir "LibCommon.psm1") -Force
Import-Module (Join-Path $LibDir "LibBuild.psm1") -Force

Describe "LibBuild" {
    It "Should clean existing build directory" {
        $tempDir = Join-Path $TestDrive "TestBuild"
        New-Item -ItemType Directory -Path $tempDir -Force | Out-Null
        New-Item -ItemType File -Path (Join-Path $tempDir "test.txt") -Force | Out-Null

        $result = Clean-BuildDir $tempDir
        $result | Should Be $true
        Test-Path $tempDir | Should Be $false
    }

    It "Should skip cleaning non-existent directory" {
        $tempDir = Join-Path $TestDrive "NonexistentBuild"

        $result = Clean-BuildDir $tempDir
        $result | Should Be $true
    }

    It "Should ensure build directory exists" {
        $tempDir = Join-Path $TestDrive "EnsureTest"

        Ensure-BuildDir $tempDir
        Test-Path $tempDir | Should Be $true
    }

    It "Should not fail if build directory already exists" {
        $tempDir = $TestDrive

        { Ensure-BuildDir $tempDir } | Should Not Throw
    }

    It "Should return positive job count" {
        $jobs = Get-ParallelJobCount
        $jobs | Should BeGreaterThan 0
    }

    It "Should detect cmake cache exists" {
        $tempDir = Join-Path $TestDrive "CmakeCacheTest"
        New-Item -ItemType Directory -Path $tempDir -Force | Out-Null
        New-Item -ItemType File -Path (Join-Path $tempDir "CMakeCache.txt") -Force | Out-Null

        $result = Test-CmakeCache -BuildDir $tempDir
        $result | Should Be $true
    }

    It "Should detect cmake cache does not exist" {
        $tempDir = Join-Path $TestDrive "NoCmakeCache"
        New-Item -ItemType Directory -Path $tempDir -Force | Out-Null

        $result = Test-CmakeCache -BuildDir $tempDir
        $result | Should Be $false
    }

    It "Should get cmake cache variable" {
        $tempDir = Join-Path $TestDrive "GetCmakeVarTest"
        New-Item -ItemType Directory -Path $tempDir -Force | Out-Null
        @"
CMAKE_GENERATOR:INTERNAL=Ninja
CMAKE_BUILD_TYPE:STRING=Release
"@ | Out-File -FilePath (Join-Path $tempDir "CMakeCache.txt") -Encoding utf8

        $result = Get-CmakeCacheVar -BuildDir $tempDir -VarName "CMAKE_GENERATOR"
        $result | Should Be "Ninja"
    }

    It "Should return empty for missing cmake cache variable" {
        $tempDir = Join-Path $TestDrive "MissingCmakeVarTest"
        New-Item -ItemType Directory -Path $tempDir -Force | Out-Null
        @"
CMAKE_GENERATOR:INTERNAL=Ninja
"@ | Out-File -FilePath (Join-Path $tempDir "CMakeCache.txt") -Encoding utf8

        $result = Get-CmakeCacheVar -BuildDir $tempDir -VarName "NONEXISTENT"
        $result | Should Be ""
    }

    It "Should handle build timer" {
        { Start-BuildTimer } | Should Not Throw
        Start-Sleep -Milliseconds 100
        { Stop-BuildTimer } | Should Not Throw
    }
}
