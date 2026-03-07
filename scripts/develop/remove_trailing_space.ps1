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

$ErrorActionPreference = "Stop"

# =============================================================================
# Color definitions
# =============================================================================
$Green = "`e[0;32m"
$Blue = "`e[0;34m"
$Yellow = "`e[0;33m"
$Red = "`e[0;31m"
$Cyan = "`e[0;36m"
$Gray = "`e[0;90m"
$Reset = "`e[0m"

# =============================================================================
# Script directory / project root
# =============================================================================
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Split-Path -Parent (Split-Path -Parent $ScriptDir)

# =============================================================================
# Logging functions
# =============================================================================
function Log-Info {
    param([string]$Message)
    Write-Host "${Gray}$Message${Reset}"
}

function Log-Success {
    param([string]$Message)
    Write-Host "${Green}$Message${Reset}"
}

function Log-Warning {
    param([string]$Message)
    Write-Host "${Yellow}$Message${Reset}"
}

function Log-Error {
    param([string]$Message)
    Write-Host "${Red}$Message${Reset}"
}

function Log-Cyan {
    param([string]$Message)
    Write-Host "${Cyan}$Message${Reset}"
}

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
            Write-Host "${Red}Unknown option: $arg${Reset}"
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
Log-Cyan "=== Remove Trailing Whitespace ==="
Log-Info "Project: $ProjectRoot"
if ($StagedOnly) {
    if ($StagedCheck) {
        Log-Info "Mode: Check staged files (pre-commit)"
    }
    else {
        Log-Info "Mode: Staged files only"
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
                        Log-Warning "${relPath}:"
                        $lineNumbers | Select-Object -First 5 | ForEach-Object {
                            Write-Host "  ${Gray}${_}:$($lines[$_ - 1])${Reset}"
                        }
                        $Changed++
                    }
                    else {
                        $newContent = $lines | ForEach-Object { $_ -replace '\s+$', '' }
                        $newContent | Out-File -FilePath $filePath -Encoding UTF8 -NoNewline
                        Log-Info "Fixed: $relPath"
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
                    Log-Warning "${relPath}:"
                    $lineNumbers | Select-Object -First 5 | ForEach-Object {
                        $lineContent = $lines[$_ - 1]
                        Write-Host "  ${Gray}${_}:$lineContent${Reset}" -NoNewline
                        Write-Host "${Red}← trailing space${Reset}"
                    }
                    $Changed++
                }
                elseif ($CheckMode) {
                    Log-Error "$relPath"
                    $Changed++
                }
                else {
                    # Remove trailing whitespace and write back
                    $newContent = $lines | ForEach-Object {
                        $_ -replace '\s+$', ''
                    }
                    $newContent | Out-File -FilePath $filePath -Encoding UTF8 -NoNewline
                    Log-Info "Fixed: $relPath"
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
Log-Cyan "=== Summary ==="
Log-Info "Processed: $Processed files"

if ($DryRun) {
    if ($Changed -gt 0) {
        Log-Warning "Files with trailing whitespace: $Changed"
    } else {
        Log-Success "No trailing whitespace found!"
    }
}
elseif ($CheckMode) {
    if ($Changed -gt 0) {
        Log-Error "Files with trailing whitespace: $Changed"
        if ($StagedCheck) {
            Log-Warning "Run with -Staged to fix staged files"
        }
        else {
            Log-Warning "Run without -Check to fix"
        }
        exit 1
    } else {
        Log-Success "No trailing whitespace found!"
    }
}
else {
    Log-Success "Fixed: $Changed files"
}

Write-Host ""
