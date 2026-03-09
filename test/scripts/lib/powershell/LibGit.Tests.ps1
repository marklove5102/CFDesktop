# =============================================================================
# Unit Tests for LibGit.psm1
# =============================================================================
#
# Usage:
#   Invoke-Pester LibGit.Tests.ps1
#
# Requires: Pester 3.4+ or 5.x
#
# =============================================================================

# 加载被测试的模块
$LibDir = Join-Path (Split-Path (Split-Path (Split-Path (Split-Path $PSScriptRoot -Parent) -Parent) -Parent) -Parent) "scripts\lib\powershell"
Import-Module (Join-Path $LibDir "LibGit.psm1") -Force

Describe "LibGit.Version" {
    It "Should extract major version" {
        $result = Get-MajorVersion -Version "1.2.3"
        $result | Should Be "1"
    }

    It "Should extract minor version" {
        $result = Get-MinorVersion -Version "1.2.3"
        $result | Should Be "2"
    }

    It "Should extract patch version" {
        $result = Get-PatchVersion -Version "1.2.3"
        $result | Should Be "3"
    }

    It "Should return empty for minor version when missing" {
        $result = Get-MinorVersion -Version "1"
        $result | Should Be ""
    }

    It "Should return empty for patch version when missing" {
        $result = Get-PatchVersion -Version "1.2"
        $result | Should Be ""
    }

    It "Should compare equal versions" {
        $result = Compare-Versions -Version1 "1.2.3" -Version2 "1.2.3"
        $result | Should Be 0
    }

    It "Should compare greater version" {
        $result = Compare-Versions -Version1 "1.3.0" -Version2 "1.2.9"
        $result | Should Be 1
    }

    It "Should compare less version" {
        $result = Compare-Versions -Version1 "1.2.9" -Version2 "1.3.0"
        $result | Should Be -1
    }

    It "Should compare versions with different lengths" {
        $result = Compare-Versions -Version1 "1.2" -Version2 "1.2.0"
        $result | Should Be 0
    }
}

Describe "LibGit.VerifyLevel" {
    It "Should return major for major version change" {
        $result = Determine-VerifyLevel -LocalVersion "1.2.3" -RemoteVersion "2.0.0"
        $result | Should Be "major"
    }

    It "Should return minor for minor version change" {
        $result = Determine-VerifyLevel -LocalVersion "1.2.3" -RemoteVersion "1.3.0"
        $result | Should Be "minor"
    }

    It "Should return patch for patch version change" {
        $result = Determine-VerifyLevel -LocalVersion "1.2.3" -RemoteVersion "1.2.4"
        $result | Should Be "patch"
    }

    It "Should handle empty local version" {
        $result = Determine-VerifyLevel -LocalVersion "" -RemoteVersion "1.2.3"
        $result | Should Be "major"
    }

    It "Should handle empty remote version" {
        $result = Determine-VerifyLevel -LocalVersion "1.2.3" -RemoteVersion ""
        $result | Should Be "major"
    }

    It "Should return description for major level" {
        $result = Get-VerifyLevelDescription -Level "major"
        $result | Should Not BeNullOrEmpty
        $result.Contains("Major") | Should Be $true
        $result.Contains("X64") | Should Be $true
        $result.Contains("ARM64") | Should Be $true
    }

    It "Should return description for minor level" {
        $result = Get-VerifyLevelDescription -Level "minor"
        $result | Should Not BeNullOrEmpty
        $result.Contains("Minor") | Should Be $true
        $result.Contains("X64") | Should Be $true
    }

    It "Should return description for patch level" {
        $result = Get-VerifyLevelDescription -Level "patch"
        $result | Should Not BeNullOrEmpty
        $result.Contains("Patch") | Should Be $true
        $result.Contains("X64") | Should Be $true
    }
}
