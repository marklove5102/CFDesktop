# This script cleans the build directory then calls the fast version to build

# 导入库模块
$LibDir = Join-Path (Split-Path -Parent $PSScriptRoot) "lib\powershell"
Import-Module (Join-Path $LibDir "LibCommon.psm1") -Force
Import-Module (Join-Path $LibDir "LibConfig.psm1") -Force
Import-Module (Join-Path $LibDir "LibPaths.psm1") -Force
Import-Module (Join-Path $LibDir "LibBuild.psm1") -Force

$ErrorActionPreference = "Stop"

# Set caller's PSScriptRoot for module functions to access
$global:CallerPSScriptRoot = $PSScriptRoot
$global:CallerMyInvocationPath = $MyInvocation.MyCommand.Path

Write-LogSeparator
Write-LogInfo "Starting Windows Build Process (Full Clean + Build)"
Write-LogSeparator

# Get the script directory and project root using library functions
$ScriptDir = Get-ScriptDir
$ProjectRoot = Get-ProjectRoot

Write-LogInfo "Project root: $ProjectRoot"
Set-Location $ProjectRoot

# Load configuration from INI file
$ConfigFile = Join-Path $ScriptDir "build_deploy_config.ini"
Write-LogInfo "Loading configuration from: $ConfigFile"

try {
    $Config = Get-IniConfig -FilePath $ConfigFile
    Write-LogSuccess "Configuration loaded successfully!"
}
catch {
    Write-LogError "Failed to load configuration: $_"
    exit 1
}

# Extract configuration values
$BuildDir = $Config["paths"]["build_dir"]
$FullBuildPath = Join-Path $ProjectRoot $BuildDir

Write-LogInfo "Build directory: $BuildDir"

# Step 1: Clean build directory using library function
Write-LogSeparator
Write-LogInfo "Step 1: Cleaning build directory"
Write-LogSeparator

if (Clean-BuildDir $FullBuildPath) {
    Write-LogSuccess "Build directory cleaned successfully!"
}
else {
    Write-LogError "Failed to clean build directory"
    exit 1
}

# Step 2: Call the fast build script
Write-LogSeparator
Write-LogInfo "Step 2: Calling fast build script"
Write-LogSeparator

$FastBuildScript = Join-Path $ScriptDir "windows_fast_deploy_build.ps1"
Write-LogInfo "Executing: $FastBuildScript"

try {
    & $FastBuildScript
    if ($LASTEXITCODE -eq 0) {
        Write-LogSeparator
        Write-LogSuccess "Build process completed successfully!"
        Write-LogSeparator
    }
    else {
        Write-LogError "Fast build script failed with exit code: $LASTEXITCODE"
        exit $LASTEXITCODE
    }
}
catch {
    Write-LogError "Error during fast build execution: $_"
    exit 1
}

# Step 3: Run tests
Write-LogSeparator
Write-LogInfo "Step 3: Running tests"
Write-LogSeparator

$TestScript = Join-Path $ScriptDir "windows_run_tests.ps1"
Write-LogInfo "Executing: $TestScript -Config deploy"

try {
    & $TestScript -Config "deploy"
    if ($LASTEXITCODE -eq 0) {
        Write-LogSeparator
        Write-LogSuccess "All tests passed successfully!"
        Write-LogSeparator
    }
    else {
        Write-LogSeparator
        Write-LogWarning "Some tests failed with exit code: $LASTEXITCODE"
        Write-LogSeparator
        # Don't exit on test failure, just warn
    }
}
catch {
    Write-LogWarning "Error during test execution: $_"
}
