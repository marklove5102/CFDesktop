# =============================================================================
# Unit Tests for LibArgs.psm1
# =============================================================================
#
# Usage:
#   Invoke-Pester LibArgs.Tests.ps1
#
# Requires: Pester 3.4+ or 5.x
#
# =============================================================================

# 加载被测试的模块
$LibDir = Join-Path (Split-Path (Split-Path (Split-Path (Split-Path $PSScriptRoot -Parent) -Parent) -Parent) -Parent) "scripts\lib\powershell"
Import-Module (Join-Path $LibDir "LibArgs.psm1") -Force

Describe "LibArgs" {
    It "Should parse develop mode" {
        $result = Parse-ConfigMode -Mode "develop"
        $result | Should Be "develop"
    }

    It "Should parse deploy mode" {
        $result = Parse-ConfigMode -Mode "deploy"
        $result | Should Be "deploy"
    }

    It "Should parse ci mode" {
        $result = Parse-ConfigMode -Mode "ci"
        $result | Should Be "ci"
    }

    It "Should return null for invalid mode" {
        $result = Parse-ConfigMode -Mode "invalid"
        $result | Should BeNullOrEmpty
    }

    It "Should validate develop mode" {
        $result = Test-ValidConfigMode -Mode "develop"
        $result | Should Be $true
    }

    It "Should validate deploy mode" {
        $result = Test-ValidConfigMode -Mode "deploy"
        $result | Should Be $true
    }

    It "Should validate ci mode" {
        $result = Test-ValidConfigMode -Mode "ci"
        $result | Should Be $true
    }

    It "Should reject invalid mode" {
        $result = Test-ValidConfigMode -Mode "invalid"
        $result | Should Be $false
    }

    It "Should detect help argument -h" {
        $result = Test-HelpArg -Arg "-h"
        $result | Should Be $true
    }

    It "Should detect help argument --help" {
        $result = Test-HelpArg -Arg "--help"
        $result | Should Be $true
    }

    It "Should detect help argument help" {
        $result = Test-HelpArg -Arg "help"
        $result | Should Be $true
    }

    It "Should reject non-help argument" {
        $result = Test-HelpArg -Arg "build"
        $result | Should Be $false
    }

    It "Should show standard usage" {
        { Show-StandardUsage } | Should Not Throw
    }

    It "Should show detailed usage" {
        { Show-DetailedUsage -Description "Test script" } | Should Not Throw
    }

    It "Should show unknown arg error" {
        { Show-UnknownArgError -Arg "--unknown" } | Should Not Throw
    }

    It "Should show missing value error" {
        { Show-MissingValueError -Arg "--config" } | Should Not Throw
    }
}
