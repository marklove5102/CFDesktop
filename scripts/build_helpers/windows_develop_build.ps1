# This script configures and builds the project using configuration from build_config.ini
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
Write-Log "Starting Windows Build Process" "INFO"
Write-Log "You shell specified all build settings in $ConfigFile before action builds..." "INFO"
Write-Log "========================================" "INFO"

# Get the script directory and project root
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
# Script is in scripts/build_helpers/, so project root is two levels up
$ProjectRoot = Split-Path -Parent (Split-Path -Parent $ScriptDir)

Write-Log "Project root: $ProjectRoot" "INFO"
Write-Log "Changing to project directory" "INFO"

Set-Location $ProjectRoot

# Load configuration from INI file
$ConfigFile = Join-Path $ScriptDir "build_develop_config.ini"
Write-Log "Loading configuration from: $ConfigFile" "INFO"

try {
    $Config = Get-IniConfig -FilePath $ConfigFile
    Write-Log "Configuration loaded successfully!" "SUCCESS"
}
catch {
    Write-Log "Failed to load configuration: $_" "ERROR"
    exit 1
}

# Extract configuration values
$Generator = $Config["cmake"]["generator"]
$Toolchain = $Config["cmake"]["toolchain"]
$SourceDir = $Config["paths"]["source"]
$BuildDir = $Config["paths"]["build_dir"]

# Resolve source directory: if relative, make it relative to project root
if ([System.IO.Path]::IsPathRooted($SourceDir)) {
    # Already an absolute path
    $ResolvedSourceDir = $SourceDir
}
else {
    # Relative path, resolve against project root
    $ResolvedSourceDir = Join-Path $ProjectRoot $SourceDir
    # Normalize the path (e.g., resolve "." to actual path)
    $ResolvedSourceDir = (Resolve-Path -Path $ResolvedSourceDir -ErrorAction SilentlyContinue).Path
    if (-not $ResolvedSourceDir) {
        # Fallback if Resolve-Path fails (directory might not exist yet)
        $ResolvedSourceDir = Join-Path $ProjectRoot $SourceDir
    }
}

Write-Log "Generator: $Generator" "INFO"
Write-Log "Toolchain: $Toolchain" "INFO"
Write-Log "Source directory: $SourceDir (resolved: $ResolvedSourceDir)" "INFO"
Write-Log "Build directory: $BuildDir" "INFO"

# Step 0: Clean build directory
Write-Log "========================================" "INFO"
Write-Log "Step 0: Cleaning build directory" "INFO"
Write-Log "Build directory path: $BuildDir" "INFO"
Write-Log "========================================" "INFO"

$FullBuildPath = Join-Path $ProjectRoot $BuildDir

# Step 0: Clear the Build Dirent
if (Test-Path $FullBuildPath) {
    Write-Log "Removing existing build directory: $FullBuildPath" "INFO"
    try {
        Remove-Item -Path $FullBuildPath -Recurse -Force
        Write-Log "Build directory cleaned successfully!" "SUCCESS"
    }
    catch {
        Write-Log "Failed to clean build directory: $_" "ERROR"
        exit 1
    }
}
else {
    Write-Log "Build directory does not exist, skipping clean step" "INFO"
}

# Step 1: Configure with CMake
Write-Log "========================================" "INFO"
Write-Log "Step 1: Configuring with CMake" "INFO"
Write-Log "Command: cmake -G $Generator -DUSE_TOOLCHAIN=$Toolchain -S $ResolvedSourceDir -B $BuildDir" "INFO"
Write-Log "========================================" "INFO"

$cmakeConfigArgs = @(
    "-G", $Generator,
    "-DUSE_TOOLCHAIN=$Toolchain",
    "-S", $ResolvedSourceDir,
    "-B", $BuildDir
)

try {
    & cmake @cmakeConfigArgs
    if ($LASTEXITCODE -eq 0) {
        Write-Log "CMake configuration completed successfully!" "SUCCESS"
    }
    else {
        Write-Log "CMake configuration failed with exit code: $LASTEXITCODE" "ERROR"
        exit $LASTEXITCODE
    }
}
catch {
    Write-Log "Error during CMake configuration: $_" "ERROR"
    exit 1
}

# Step 2: Build with CMake
Write-Log "========================================" "INFO"
Write-Log "Step 2: Building project" "INFO"
Write-Log "Command: cmake --build $BuildDir" "INFO"
Write-Log "========================================" "INFO"

try {
    & cmake --build $BuildDir
    if ($LASTEXITCODE -eq 0) {
        Write-Log "Build completed successfully!" "SUCCESS"
    }
    else {
        Write-Log "Build failed with exit code: $LASTEXITCODE" "ERROR"
        exit $LASTEXITCODE
    }
}
catch {
    Write-Log "Error during build: $_" "ERROR"
    exit 1
}

Write-Log "========================================" "INFO"
Write-Log "Build process completed successfully!" "SUCCESS"
Write-Log "========================================" "INFO"
