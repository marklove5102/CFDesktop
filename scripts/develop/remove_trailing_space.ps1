# =============================================================================
# Remove Trailing Whitespace Script (Windows PowerShell)
# =============================================================================
#
# Usage: .\scripts\develop\remove_trailing_space.ps1 [OPTIONS]
#
# Options:
#   -DryRun       Show what would be changed without modifying files
#   -Check        Return exit code 1 if any files have trailing whitespace
#   -Staged       Fix staged files (auto-fix trailing whitespace)
#   -StagedCheck  Check staged files without modifying (for pre-commit hook)
#   -Help         Show this help message
#
# =============================================================================

# 导入库模块
$LibDir = Join-Path (Split-Path -Parent $PSScriptRoot) "lib\powershell"
Import-Module (Join-Path $LibDir "LibCommon.psm1") -Force
Import-Module (Join-Path $LibDir "LibPaths.psm1") -Force

$ErrorActionPreference = "Stop"

# Set caller's PSScriptRoot for module functions to access
$global:CallerPSScriptRoot = $PSScriptRoot
$global:CallerMyInvocationPath = $MyInvocation.MyCommand.Path

# =============================================================================
# Script directory / project root using library functions
# =============================================================================
$ScriptDir = Get-ScriptDir
$ProjectRoot = Get-ProjectRoot

# =============================================================================
# Show help
# =============================================================================
function Show-Help {
    Write-Host ""
    Write-Host "Usage: .\scripts\develop\remove_trailing_space.ps1 [OPTIONS]"
    Write-Host ""
    Write-Host "Options:"
    Write-Host "  -DryRun       Show what would be changed without modifying files"
    Write-Host "  -Check        Return exit code 1 if any files have trailing whitespace"
    Write-Host "  -Staged       Fix staged files (auto-fix trailing whitespace)"
    Write-Host "  -StagedCheck  Check staged files without modifying (for pre-commit hook)"
    Write-Host "  -Help         Show this help message"
    Write-Host ""
    exit 0
}

# =============================================================================
# Parse arguments
# =============================================================================
$DryRun = $false
$CheckMode = $false
$StagedOnly = $false
$StagedCheck = $false

foreach ($arg in $args) {
    switch -Regex ($arg) {
        "^-n|--dry-run|^-DryRun$" {
            $DryRun = $true
        }
        "^-c|--check|^-Check$" {
            $CheckMode = $true
        }
        "^-StagedCheck$|--staged-check|^-S$" {
            $StagedCheck = $true
            $StagedOnly = $true
            $CheckMode = $true
        }
        "^-Staged$|--staged|^-s$" {
            $StagedOnly = $true
        }
        "^-h|--help|^-Help$" {
            Show-Help
        }
        default {
            Write-Host ""
            Write-LogError "Unknown option: $arg"
            Show-Help
        }
    }
}

# =============================================================================
# Excluded file extensions (binary files that git tracks but we shouldn't check)
# =============================================================================
$ExcludeExtensions = @(
    # Images
    ".png", ".jpg", ".jpeg", ".gif", ".ico", ".bmp", ".svg",
    # Archives
    ".pdf", ".zip", ".tar", ".gz", ".7z", ".rar",
    # Fonts
    ".ttf", ".otf", ".woff", ".woff2", ".eot"
)

# =============================================================================
# Print header
# =============================================================================
Write-Host ""
Write-LogInfo "=== Remove Trailing Whitespace ==="
Write-Host "Project: $ProjectRoot" -ForegroundColor Gray
if ($StagedOnly) {
    if ($StagedCheck) {
        Write-Host "Mode: Check staged files (pre-commit)" -ForegroundColor Gray
    }
    else {
        Write-Host "Mode: Staged files only" -ForegroundColor Gray
    }
}
Write-Host ""

# =============================================================================
# Process files
# =============================================================================
$Processed = 0
$Changed = 0

if ($StagedOnly) {
    # Get staged files from git
    $stagedFiles = git diff --cached --name-only --diff-filter=ACM 2>$null
    if ($stagedFiles) {
        foreach ($relFile in $stagedFiles) {
            if ([string]::IsNullOrWhiteSpace($relFile)) { continue }
            $filePath = Join-Path $ProjectRoot $relFile
            if (!(Test-Path $filePath -PathType Leaf)) { continue }

            $relPath = $relFile.Replace('\', '/')
            $Processed++

            # Skip excluded extensions
            $ext = [System.IO.Path]::GetExtension($filePath)
            if ($ExcludeExtensions -contains $ext.ToLower()) { continue }

            # Check for trailing whitespace
            try {
                $lines = Get-Content $filePath -Encoding UTF8
                $hasTrailing = $false
                $lineNumbers = @()

                for ($i = 0; $i -lt $lines.Count; $i++) {
                    if ($lines[$i] -match '\s+$') {
                        $hasTrailing = $true
                        $lineNumbers += ($i + 1)
                    }
                }

                if ($hasTrailing) {
                    if ($DryRun -or $CheckMode) {
                        Write-LogWarning "${relPath}:"
                        $lineNumbers | Select-Object -First 5 | ForEach-Object {
                            Write-Host "  $($_):$($lines[$_ - 1])" -ForegroundColor Gray
                        }
                        $Changed++
                    }
                    else {
                        $newContent = $lines | ForEach-Object { $_ -replace '\s+$', '' }
                        $newContent | Out-File -FilePath $filePath -Encoding UTF8 -NoNewline
                        Write-Host "Fixed: $relPath" -ForegroundColor Gray
                        $Changed++
                    }
                }
            }
            catch {
                # Skip files that can't be read as text
            }
        }
    }
}
else {
    # Use git ls-files to get tracked files (respects .gitignore)
    git ls-files | ForEach-Object {
        $relPath = $_
        $filePath = Join-Path $ProjectRoot $relPath

        # Skip excluded extensions (binary files)
        $ext = [System.IO.Path]::GetExtension($filePath)
        if ($ExcludeExtensions -contains $ext.ToLower()) { return }

        $Processed++

        # Read file content and check for trailing whitespace
        try {
            $lines = Get-Content $filePath -Encoding UTF8
            $hasTrailing = $false
            $lineNumbers = @()

            for ($i = 0; $i -lt $lines.Count; $i++) {
                if ($lines[$i] -match '\s+$') {
                    $hasTrailing = $true
                    $lineNumbers += ($i + 1)
                }
            }

            if ($hasTrailing) {
                if ($DryRun) {
                    Write-LogWarning "${relPath}:"
                    $lineNumbers | Select-Object -First 5 | ForEach-Object {
                        $lineContent = $lines[$_ - 1]
                        Write-Host "  ${_}:$lineContent" -NoNewline -ForegroundColor Gray
                        Write-Host " ← trailing space" -ForegroundColor Red
                    }
                    $Changed++
                }
                elseif ($CheckMode) {
                    Write-LogError "$relPath"
                    $Changed++
                }
                else {
                    # Remove trailing whitespace and write back
                    $newContent = $lines | ForEach-Object {
                        $_ -replace '\s+$', ''
                    }
                    $newContent | Out-File -FilePath $filePath -Encoding UTF8 -NoNewline
                    Write-Host "Fixed: $relPath" -ForegroundColor Gray
                    $Changed++
                }
            }
        }
        catch {
            # Skip files that can't be read as text (binary files)
        }
    }
}

# =============================================================================
# Print summary
# =============================================================================
Write-Host ""
Write-LogInfo "=== Summary ==="
Write-Host "Processed: $Processed files" -ForegroundColor Gray

if ($DryRun) {
    if ($Changed -gt 0) {
        Write-LogWarning "Files with trailing whitespace: $Changed"
    } else {
        Write-LogSuccess "No trailing whitespace found!"
    }
}
elseif ($CheckMode) {
    if ($Changed -gt 0) {
        Write-LogError "Files with trailing whitespace: $Changed"
        if ($StagedCheck) {
            Write-LogWarning "Run with -Staged to fix staged files"
        }
        else {
            Write-LogWarning "Run without -Check to fix"
        }
        exit 1
    } else {
        Write-LogSuccess "No trailing whitespace found!"
    }
}
else {
    Write-LogSuccess "Fixed: $Changed files"
}

Write-Host ""
