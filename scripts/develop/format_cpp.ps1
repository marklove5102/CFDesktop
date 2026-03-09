#!/usr/bin/env pwsh
<#
.SYNOPSIS
    Format all C++ files in the project using clang-format

.DESCRIPTION
    This script formats all C++ source files (.cpp, .h, .hpp, .cc, .cxx)
    in the project using clang-format, excluding third_party and build dirs.

.PARAMETER DryRun
    Show what would be changed without modifying files

.PARAMETER Check
    Return exit code 1 if any files need formatting

.EXAMPLE
    .\scripts\develop\format_cpp.ps1
    Format all C++ files

.EXAMPLE
    .\scripts\develop\format_cpp.ps1 -DryRun
    Show what would be formatted

.EXAMPLE
    .\scripts\develop\format_cpp.ps1 -Check
    Check if files need formatting (for CI)
#>

param(
    [switch]$DryRun,
    [switch]$Check
)

# 导入库模块
$LibDir = Join-Path (Split-Path -Parent $PSScriptRoot) "lib\powershell"
Import-Module (Join-Path $LibDir "LibCommon.psm1") -Force
Import-Module (Join-Path $LibDir "LibPaths.psm1") -Force

# Set caller's PSScriptRoot for module functions to access
$global:CallerPSScriptRoot = $PSScriptRoot
$global:CallerMyInvocationPath = $MyInvocation.MyCommand.Path

# Check if clang-format exists
function Test-ClangFormat {
    $clangFormat = Get-Command "clang-format" -ErrorAction SilentlyContinue
    if ($null -eq $clangFormat) {
        Write-LogError "ERROR: clang-format not found!"
        Write-LogWarning "Please install clang-format:"
        Write-Host "  - Windows: LLVM installer or apt install clang-format" -ForegroundColor Gray
        Write-Host "  - Or add to PATH" -ForegroundColor Gray
        return $false
    }

    $version = & clang-format --version
    Write-LogSuccess "Found: $version"
    return $true
}

# Get all C++ files (excluding third_party and build directories)
function Get-CppFiles {
    $rootPath = Get-ProjectRoot

    $excludePatterns = @(
        "*third_party*",
        "*build*",
        "*out*",
        "*.git*"
    )

    $files = Get-ChildItem -Path $rootPath -Recurse -Include @("*.cpp", "*.h", "*.hpp", "*.cc", "*.cxx") |
        Where-Object {
            $file = $_
            $shouldExclude = $false
            foreach ($pattern in $excludePatterns) {
                if ($file.FullName -like "*$pattern*") {
                    $shouldExclude = $true
                    break
                }
            }
            -not $shouldExclude
        }

    return $files
}

# Main
$ErrorActionPreference = "Stop"

Write-Host ""
Write-LogInfo "=== C++ Code Formatter ==="
Write-Host "Project: CFDesktop" -ForegroundColor Gray

if (-not (Test-ClangFormat)) {
    exit 1
}

$files = Get-CppFiles
$fileCount = $files.Count

if ($fileCount -eq 0) {
    Write-LogWarning "No C++ files found!"
    exit 0
}

Write-Host ""
Write-Host "Found $fileCount C++ files to process" -ForegroundColor Gray
Write-Host ""

$processed = 0
$changed = 0

foreach ($file in $files) {
    $processed++
    $relativePath = $file.FullName.Substring((Get-Location).Path.Length + 1)

    if ($DryRun) {
        # Check if file would be changed
        $original = Get-Content $file.FullName -Raw
        $formatted = & clang-format $file.FullName

        if ($original -ne $formatted) {
            Write-LogWarning "[$processed/$fileCount] Would reformat: $relativePath"
            $changed++
        } else {
            Write-Host "[$processed/$fileCount] OK: $relativePath"
        }
    }
    elseif ($Check) {
        # Check mode: return error if files need formatting
        $original = Get-Content $file.FullName -Raw
        $formatted = & clang-format $file.FullName

        if ($original -ne $formatted) {
            Write-LogError "[$processed/$fileCount] Needs formatting: $relativePath"
            $changed++
        } else {
            Write-Host "[$processed/$fileCount] OK: $relativePath"
        }
    }
    else {
        # Format in-place
        Write-Host "[$processed/$fileCount] Formatting: $relativePath"
        & clang-format -i $file.FullName
        $changed++
    }
}

Write-Host ""
Write-LogInfo "=== Summary ==="
Write-Host "Processed: $processed / $fileCount files" -ForegroundColor Gray

if ($DryRun) {
    Write-LogWarning "Would reformat: $changed files"
}
elseif ($Check) {
    if ($changed -gt 0) {
        Write-LogError "Files needing formatting: $changed"
        Write-LogWarning "Run without -Check to format"
        exit 1
    } else {
        Write-LogSuccess "All files formatted correctly!"
    }
}
else {
    Write-LogSuccess "Formatted: $changed files"
}

exit 0
