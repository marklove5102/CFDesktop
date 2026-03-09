# =============================================================================
# CFDesktop Scripts Library - Test Runner (PowerShell)
# =============================================================================
#
# 运行所有 PowerShell 库的单元测试
#
# 用法:
#   .\run_all_tests.ps1
#   .\run_all_tests.ps1 -Verbose
#
# =============================================================================

[CmdletBinding()]
param()

# =============================================================================
# 初始化
# =============================================================================

$ErrorActionPreference = "Continue"
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$LibTestDir = Join-Path $ScriptDir "lib\powershell"

$script:TotalTests = 0
$script:PassedTests = 0
$script:FailedTests = 0
$script:FailedTestList = @()
$script:TestResults = @()

# =============================================================================
# 输出函数
# =============================================================================

function Write-TestSeparator {
    param(
        [string]$Char = '=',
        [int]$Width = 80
    )
    $line = $Char * $Width
    Write-Host $line -ForegroundColor Gray
}

function Write-TestHeader {
    param([string]$Text)

    Write-Host ""
    Write-TestSeparator
    Write-Host $Text -ForegroundColor Cyan
    Write-TestSeparator
    Write-Host ""
}

function Write-TestStart {
    param([string]$TestName)

    Write-Host "▶ Running: " -NoNewline
    Write-Host $TestName -ForegroundColor Cyan
    Write-Host (" " * 80) -ForegroundColor Gray
}

function Write-TestPass {
    param(
        [string]$TestName,
        [timespan]$Duration
    )

    $durationText = if ($Duration.TotalSeconds -lt 1) {
        "$($Duration.TotalMilliseconds.ToString('0'))ms"
    } else {
        "$($Duration.TotalSeconds.ToString('0.00'))s"
    }

    Write-Host "✓ PASSED: " -NoNewline -ForegroundColor Green
    Write-Host "$TestName " -NoNewline
    Write-Host "($durationText)" -ForegroundColor Gray
    Write-Host ""
}

function Write-TestFail {
    param(
        [string]$TestName,
        [timespan]$Duration
    )

    $durationText = if ($Duration.TotalSeconds -lt 1) {
        "$($Duration.TotalMilliseconds.ToString('0'))ms"
    } else {
        "$($Duration.TotalSeconds.ToString('0.00'))s"
    }

    Write-Host "✗ FAILED: " -NoNewline -ForegroundColor Red
    Write-Host "$TestName " -NoNewline
    Write-Host "($durationText)" -ForegroundColor Gray
    Write-Host ""
}

function Write-TestWarning {
    param([string]$Message)

    Write-Host "⚠ WARNING: " -NoNewline -ForegroundColor Yellow
    Write-Host $Message
}

# =============================================================================
# Pester 检测
# =============================================================================

function Test-PesterInstalled {
    # 检查 Pester 是否可用
    $pesterModule = Get-Module -ListAvailable -Name Pester -ErrorAction SilentlyContinue | Sort-Object Version -Descending | Select-Object -First 1

    if (!$pesterModule) {
        Write-TestWarning "Pester is not installed!"
        Write-Host ""
        Write-Host "To install Pester, run:" -ForegroundColor Yellow
        Write-Host "  Install-Module -Name Pester -Force -Scope CurrentUser" -ForegroundColor Cyan
        Write-Host ""
        return $false
    }

    Write-Host "  Pester version: $($pesterModule.Version)" -ForegroundColor Gray

    # 确保 Pester 已加载（加载最新版本）
    if (!(Get-Module -Name Pester -ErrorAction SilentlyContinue)) {
        try {
            Import-Module Pester -RequiredVersion $pesterModule.Version -ErrorAction Stop
        }
        catch {
            Write-TestWarning "Failed to import Pester: $_"
            return $false
        }
    }

    return $true
}

# =============================================================================
# 运行单个测试
# =============================================================================

function Invoke-SingleTest {
    param([string]$TestFile)

    $testName = Split-Path $TestFile -Leaf
    Write-TestStart $testName

    # 检测 Pester 版本
    $pesterModule = Get-Module Pester
    $pesterVersion = $pesterModule.Version

    # 记录开始时间
    $stopwatch = [System.Diagnostics.Stopwatch]::StartNew()

    # 根据版本使用不同的 API
    if ($pesterVersion.Major -ge 5) {
        # Pester 5.x API
        $pesterConfig = New-PesterConfiguration
        $pesterConfig.Run.Path = $TestFile
        $pesterConfig.Output.Verbosity = 'Detailed'
        $pesterConfig.TestResult.Enabled = $true
        $result = Invoke-Pester -Configuration $pesterConfig

        $script:TotalTests += $result.TotalCount
        $script:PassedTests += $result.PassedCount
        $script:FailedTests += $result.FailedCount

        # 记录测试结果详情
        foreach ($testResult in $result.Tests) {
            $script:TestResults += @{
                TestFile = $testName
                Name = $testResult.Name
                Result = if ($testResult.Status -eq 'Passed') { 'Pass' } else { 'Fail' }
                Duration = $testResult.Duration
                Error = if ($testResult.Error) { $testResult.Error.ToString() } else { '' }
            }
        }

        if ($result.FailedCount -eq 0) {
            Write-TestPass $testName $stopwatch.Elapsed
            return $true
        } else {
            $script:FailedTestList += $testName
            Write-TestFail $testName $stopwatch.Elapsed

            Write-Host "  Failed tests:" -ForegroundColor Red
            foreach ($test in $result.Tests) {
                if ($test.Status -ne 'Passed') {
                    Write-Host "    ✗ $($test.Name)" -ForegroundColor Red
                }
            }
            Write-Host ""
            return $false
        }
    }
    else {
        # Pester 3.x / 4.x API
        $result = Invoke-Pester -Script $TestFile -PassThru

        if ($result) {
            $script:TotalTests += $result.TotalCount
            $script:PassedTests += $result.PassedCount
            $script:FailedTests += $result.FailedCount

            # 记录测试结果详情
            foreach ($testResult in $result.TestResult) {
                $script:TestResults += @{
                    TestFile = $testName
                    Name = $testResult.Name
                    Result = if ($testResult.Result -eq 'Passed') { 'Pass' } else { 'Fail' }
                    Duration = [TimeSpan]::FromMilliseconds($testResult.Time.TotalMilliseconds)
                    Error = if ($testResult.ErrorRecord) { $testResult.ErrorRecord.ToString() } else { '' }
                }
            }

            $stopwatch.Stop()

            if ($result.FailedCount -eq 0) {
                Write-TestPass $testName $stopwatch.Elapsed
                return $true
            } else {
                $script:FailedTestList += $testName
                Write-TestFail $testName $stopwatch.Elapsed

                Write-Host "  Failed tests:" -ForegroundColor Red
                foreach ($test in $result.TestResult) {
                    if ($test.Result -ne 'Passed') {
                        Write-Host "    ✗ $($test.Name)" -ForegroundColor Red
                    }
                }
                Write-Host ""
                return $false
            }
        } else {
            $stopwatch.Stop()
            Write-TestWarning "No result returned from Pester"
            return $false
        }
    }
}

# =============================================================================
# 运行所有测试
# =============================================================================

function Invoke-AllTests {
    Write-TestHeader "CFDesktop Scripts Library - PowerShell Unit Tests"

    # 检查 Pester
    if (!(Test-PesterInstalled)) {
        Write-TestSeparator
        Write-Host "Exiting due to missing Pester installation." -ForegroundColor Red
        exit 1
    }

    Write-Host "  Test Directory: " -NoNewline -ForegroundColor Gray
    Write-Host $LibTestDir -ForegroundColor White

    # 检查测试目录是否存在
    if (!(Test-Path $LibTestDir)) {
        Write-TestWarning "Test directory not found: $LibTestDir"
        exit 1
    }

    Write-Host "  Timestamp: " -NoNewline -ForegroundColor Gray
    Write-Host (Get-Date -Format "yyyy-MM-dd HH:mm:ss") -ForegroundColor White
    Write-Host ""

    # 查找所有测试文件
    $testFiles = Get-ChildItem -Path $LibTestDir -Filter "*.Tests.ps1" -File -ErrorAction SilentlyContinue | Sort-Object Name

    if (!$testFiles -or $testFiles.Count -eq 0) {
        Write-TestWarning "No test files found in $LibTestDir"
        Write-Host ""
        Write-Host "Looking for files matching pattern: *.Tests.ps1" -ForegroundColor Gray
        $allFiles = Get-ChildItem -Path $LibTestDir -File -ErrorAction SilentlyContinue
        if ($allFiles) {
            Write-Host "Files found in directory:" -ForegroundColor Gray
            $allFiles | ForEach-Object { Write-Host "  - $($_.Name)" -ForegroundColor Gray }
        }
        exit 1
    }

    Write-Host "  Found $($testFiles.Count) test file(s):" -ForegroundColor Gray
    foreach ($f in $testFiles) {
        Write-Host "    - $($f.Name)" -ForegroundColor Gray
    }
    Write-Host ""

    # 运行每个测试
    foreach ($testFile in $testFiles) {
        try {
            Invoke-SingleTest $testFile.FullName
        }
        catch {
            Write-TestWarning "Error running test $($testFile.Name): $_"
        }
    }

    # 打印汇总
    Write-TestSummary
}

# =============================================================================
# 打印测试汇总
# =============================================================================

function Write-TestSummary {
    Write-Host ""
    Write-TestSeparator
    Write-Host "Test Summary" -ForegroundColor Cyan
    Write-TestSeparator
    Write-Host ""

    # 测试统计
    Write-Host "  Total Tests:    " -NoNewline -ForegroundColor Gray
    Write-Host $script:TotalTests -ForegroundColor White
    Write-Host "  Passed:         " -NoNewline -ForegroundColor Green
    Write-Host $script:PassedTests -ForegroundColor Green
    Write-Host "  Failed:         " -NoNewline -ForegroundColor Red
    Write-Host $script:FailedTests -ForegroundColor Red

    # 通过率
    if ($script:TotalTests -gt 0) {
        $passRate = [math]::Round(($script:PassedTests / $script:TotalTests) * 100, 1)
        $passRateColor = if ($passRate -eq 100) { 'Green' } elseif ($passRate -ge 80) { 'Yellow' } else { 'Red' }
        Write-Host "  Pass Rate:      " -NoNewline -ForegroundColor Gray
        Write-Host "$passRate%" -ForegroundColor $passRateColor
    }

    Write-Host ""

    # 失败的测试
    if ($script:FailedTestList.Count -gt 0) {
        Write-Host "Failed Tests:" -ForegroundColor Red
        foreach ($testName in $script:FailedTestList) {
            Write-Host "  ✗ $testName" -ForegroundColor Red
        }
        Write-Host ""
    }

    # 测试详情审计
    if ($PSBoundParameters.ContainsKey('Verbose') -or $VerbosePreference -eq 'Continue') {
        Write-TestSeparator '-'
        Write-Host "Test Details Audit" -ForegroundColor Cyan
        Write-TestSeparator '-'
        Write-Host ""

        foreach ($result in $script:TestResults) {
            $statusColor = if ($result.Result -eq 'Pass') { 'Green' } else { 'Red' }
            $statusSymbol = if ($result.Result -eq 'Pass') { '[PASS]' } else { '[FAIL]' }

            Write-Host "  [$($result.TestFile)] " -NoNewline -ForegroundColor Gray
            Write-Host "$statusSymbol $($result.Name) " -NoNewline -ForegroundColor $statusColor
            Write-Host "($($result.Duration.TotalMilliseconds.ToString('0'))ms)" -ForegroundColor Gray

            if ($result.Result -eq 'Fail' -and $result.Error) {
                Write-Host "    Error: $($result.Error)" -ForegroundColor Red
            }
        }
        Write-Host ""
    }

    # 最终结果
    Write-TestSeparator
    if ($script:FailedTests -eq 0) {
        Write-Host "[PASS] All tests passed!" -ForegroundColor Green
        Write-TestSeparator
        Write-Host ""
        exit 0
    } else {
        Write-Host "[FAIL] Some tests failed!" -ForegroundColor Red
        Write-TestSeparator
        Write-Host ""
        exit 1
    }
}

# =============================================================================
# 主函数
# =============================================================================

Invoke-AllTests
