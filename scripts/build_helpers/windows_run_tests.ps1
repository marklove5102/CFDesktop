# This script runs CMake tests using CTest
# It reads the build directory from the specified config file

param(
    [Parameter(Mandatory = $false)]
    [string]$Config = "develop"
)

$ErrorActionPreference = "Stop"

# Function to read INI configuration file
function Get-IniConfig {
    param(
        [string]$FilePath
    )

    $config = @{}
    $currentSection = ""

    if (Test-Path $FilePath) {
        Get-Content $FilePath | ForEach-Object {
            $line = $_.Trim()

            # Skip empty lines and comments
            if ([string]::IsNullOrEmpty($line) -or $line.StartsWith("#") -or $line.StartsWith(";")) {
                return
            }

            # Section header
            if ($line -match '^\[([^\]]+)\]$') {
                $currentSection = $matches[1]
                if (-not $config.ContainsKey($currentSection)) {
                    $config[$currentSection] = @{}
                }
                return
            }

            # Key=value pair
            if ($line -match '^([^=]+)=(.*)$') {
                $key = $matches[1].Trim()
                $value = $matches[2].Trim()

                if ($currentSection -and $config.ContainsKey($currentSection)) {
                    $config[$currentSection][$key] = $value
                }
            }
        }
    }
    else {
        throw "Configuration file not found: $FilePath"
    }

    return $config
}

# Log function
function Write-Log {
    param(
        [string]$Message,
        [string]$Level = "INFO"
    )
    $timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    $color = switch ($Level) {
        "INFO" { "Cyan" }
        "SUCCESS" { "Green" }
        "WARNING" { "Yellow" }
        "ERROR" { "Red" }
        default { "White" }
    }
    Write-Host "[$timestamp] [$Level] $Message" -ForegroundColor $color
}

Write-Log "========================================" "INFO"
Write-Log "Running Tests (Config: $Config)" "INFO"
Write-Log "========================================" "INFO"

# Get the script directory and project root
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Split-Path -Parent (Split-Path -Parent $ScriptDir)

Write-Log "Project root: $ProjectRoot" "INFO"
Set-Location $ProjectRoot

# Determine which config file to use
$ConfigFile = switch ($Config) {
    "develop" { "build_develop_config.ini" }
    "deploy" { "build_deploy_config.ini" }
    default {
        Write-Log "Unknown config: $Config. Valid options are: develop, deploy" "ERROR"
        exit 1
    }
}

$ConfigFilePath = Join-Path $ScriptDir $ConfigFile
Write-Log "Loading configuration from: $ConfigFilePath" "INFO"

try {
    $ConfigData = Get-IniConfig -FilePath $ConfigFilePath
    Write-Log "Configuration loaded successfully!" "SUCCESS"
}
catch {
    Write-Log "Failed to load configuration: $_" "ERROR"
    exit 1
}

# Get build directory from config
$BuildDir = $ConfigData["paths"]["build_dir"]
$BuildDir = Join-Path $ProjectRoot $BuildDir "test"

Write-Log "Test directory: $BuildDir" "INFO"
Write-Log "Command: ctest --test-dir $BuildDir --output-on-failure" "INFO"

# Check if build directory exists
if (-not (Test-Path $BuildDir)) {
    Write-Log "Build directory does not exist: $BuildDir" "ERROR"
    Write-Log "Please run the build script first before running tests." "ERROR"
    exit 1
}

# Run tests
Write-Log "========================================" "INFO"
Write-Log "Running tests..." "INFO"
Write-Log "========================================" "INFO"

try {
    & ctest --test-dir $BuildDir --output-on-failure
    $exitCode = $LASTEXITCODE

    if ($exitCode -eq 0) {
        Write-Log "========================================" "INFO"
        Write-Log "All tests passed successfully!" "SUCCESS"
        Write-Log "========================================" "INFO"
        exit 0
    }
    else {
        Write-Log "========================================" "INFO"
        Write-Log "Some tests failed with exit code: $exitCode" "WARNING"
        Write-Log "========================================" "INFO"
        exit $exitCode
    }
}
catch {
    Write-Log "Error during test execution: $_" "ERROR"
    exit 1
}
