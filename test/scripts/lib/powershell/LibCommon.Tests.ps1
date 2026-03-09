# =============================================================================
# Unit Tests for LibCommon.psm1
# =============================================================================
#
# Usage:
#   Invoke-Pester LibCommon.Tests.ps1
#
# Requires: Pester 3.4+ or 5.x
#
# =============================================================================

# 加载被测试的模块
$LibDir = Join-Path (Split-Path (Split-Path (Split-Path (Split-Path $PSScriptRoot -Parent) -Parent) -Parent) -Parent) "scripts\lib\powershell"
Import-Module (Join-Path $LibDir "LibCommon.psm1") -Force

Describe "LibCommon" {
    It "Should write log with INFO level" {
        $output = Write-LogInfo "test message" 6>&1
        $output | Should Not BeNullOrEmpty
    }

    It "Should write log with SUCCESS level" {
        $output = Write-LogSuccess "test message" 6>&1
        $output | Should Not BeNullOrEmpty
    }

    It "Should write log with WARNING level" {
        $output = Write-LogWarning "test message" 6>&1
        $output | Should Not BeNullOrEmpty
    }

    It "Should write log with ERROR level" {
        $output = Write-LogError "test message" 6>&1
        $output | Should Not BeNullOrEmpty
    }

    It "Should write log separator" {
        $output = Write-LogSeparator 6>&1
        $output | Should Not BeNullOrEmpty
    }

    It "Should write log progress" {
        $output = Write-LogProgress -Current 5 -Total 10 -Message "Processing" 6>&1
        $output | Should Not BeNullOrEmpty
        $output | Should Match "[5/10]"
    }
}
