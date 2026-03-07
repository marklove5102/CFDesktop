# =============================================================================
# CFDesktop Docker Build Environment
# =============================================================================
#
# Usage:
#   .\scripts\build_helpers\docker_start.ps1 [options]
#
# Options:
#   -Arch <amd64|arm64>  Target architecture (default: amd64)
#   -FastBuild           Skip image cleanup, reuse existing if available
#   -Interactive         Enter interactive shell (default behavior)
#   -Verify              Run CI build verification instead of shell
#   -BuildProject        Build image + run full clean build
#   -BuildProjectFast    Build image + run fast build (no clean)
#   -RunProjectTest      Build image + run tests
#   -StayOnError         Stay in container on CI failure for debugging
#   -NoLog               Disable file logging (default: log to file)
#   -Help                Show this help message
#
# Examples:
#   .\docker_start.ps1                # Clean build + interactive shell
#   .\docker_start.ps1 -Verify        # Run CI build
#   .\docker_start.ps1 -Verify -StayOnError  # Debug on error
#   .\docker_start.ps1 -BuildProject  # Build image + full clean build
#   .\docker_start.ps1 -BuildProjectFast  # Build image + fast build
#   .\docker_start.ps1 -RunProjectTest  # Build image + run tests
#   .\docker_start.ps1 -Arch arm64    # ARM64 build (QEMU emulation)
# =============================================================================

param(
    [ValidateSet("amd64", "arm64")]
    [string]$Arch = "amd64",
    [switch]$FastBuild,
    [switch]$Interactive,
    [switch]$Verify,
    [switch]$BuildProject,
    [switch]$BuildProjectFast,
    [switch]$RunProjectTest,
    [switch]$StayOnError,
    [switch]$NoLog,
    [switch]$Help
)

$ErrorActionPreference = "Stop"

# =============================================================================
# Configuration
# =============================================================================
$Script:Config = @{
    ImageName      = "cfdesktop-build"
    ProjectName    = "CFDesktop"
    EnableLog      = -not $NoLog.IsPresent
    LogFile        = ""
    LogDir         = ""
    OriginalOutput = ""
    OriginalError  = ""
    Theme          = @{
        Arrow = "→"; Check = "✓"; Cross = "✗"; Dot = "●"; Bold = "━"
    }
    Colors         = @{
        Header  = "Cyan"
        Stage   = "Cyan"
        Success = "Green"
        Error   = "Red"
        Warn    = "Yellow"
        Dim     = "DarkGray"
        Accent  = "Blue"
        Step    = "White"
    }
}

# =============================================================================
# UI helpers
# =============================================================================
function Show-Header {
    param([string]$Title)
    $w = 60
    $line = $Script:Config.Theme.Bold * $w
    Write-Host ""
    Write-Host $line -ForegroundColor $Script:Config.Colors.Header
    $pad = " " * [Math]::Max(0, [Math]::Floor(($w - $Title.Length) / 2))
    Write-Host "${pad}${Title}" -ForegroundColor $Script:Config.Colors.Header
    Write-Host $line -ForegroundColor $Script:Config.Colors.Header
    Write-Host ""
}

function Show-Stage {
    param([string]$Name, [string]$Desc = "")
    Write-Host ""
    Write-Host "  $($Script:Config.Theme.Dot) [$Name] $Desc" -ForegroundColor $Script:Config.Colors.Stage
    Write-Host ("  " + $Script:Config.Theme.Bold * 56) -ForegroundColor $Script:Config.Colors.Dim
}

function Show-Success { param([string]$M); Write-Host "  $($Script:Config.Theme.Check) $M" -ForegroundColor $Script:Config.Colors.Success }
function Show-Error { param([string]$M); Write-Host "  $($Script:Config.Theme.Cross) $M" -ForegroundColor $Script:Config.Colors.Error }
function Show-Warn { param([string]$M); Write-Host "  [!] $M" -ForegroundColor $Script:Config.Colors.Warn }
function Show-Info { param([string]$M); Write-Host "  $($Script:Config.Theme.Arrow) $M" -ForegroundColor $Script:Config.Colors.Step }

# =============================================================================
# Logging setup
# =============================================================================
function Initialize-Logging {
    if (-not $Script:Config.EnableLog) { return }

    $ProjectRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
    $Script:Config.LogDir = Join-Path $ProjectRoot "scripts\docker\logger"

    # Create logger directory if it doesn't exist
    if (-not (Test-Path $Script:Config.LogDir)) {
        New-Item -ItemType Directory -Path $Script:Config.LogDir -Force | Out-Null
    }

    # Generate timestamp: YYYYMMDD_HHMMSS
    $Timestamp = Get-Date -Format "yyyyMMdd_HHmmss"
    $Script:Config.LogFile = Join-Path $Script:Config.LogDir "ci_build_${Timestamp}.log"

    # Initialize log file with header
    $Header = @"
===============================================================================
CFDesktop Docker Build Log
===============================================================================
Start Time: $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")
Architecture: $Arch
Platform: $DockerPlatform
Fast Build: $($FastBuild.IsPresent)
Verify Mode: $RunVerify
===============================================================================
"@
    $Header | Out-File -FilePath $Script:Config.LogFile -Encoding utf8

    # Redirect output to both console and log file using Start-Transcript
    Start-Transcript -Path $Script:Config.LogFile -Append | Out-Null
}

function Close-Logging {
    if ($Script:Config.EnableLog -and $Script:Config.LogFile) {
        # Stop transcript first to release file lock
        Stop-Transcript -ErrorAction SilentlyContinue | Out-Null

        # Now we can safely append the footer
        $Footer = @"

===============================================================================
End Time: $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")
Log saved to: $($Script:Config.LogFile)
===============================================================================
"@
        $Footer | Out-File -FilePath $Script:Config.LogFile -Append -Encoding utf8 -ErrorAction SilentlyContinue
    }
}

# =============================================================================
# Docker build output formatter
# =============================================================================
function Format-DockerBuildLine {
    param([string]$Line)
    if ([string]::IsNullOrWhiteSpace($Line)) { return }

    # BuildKit step
    if ($Line -match '^#\d+\s+\[([^\]]+)\]\s*(.*)$') {
        $cmd = $matches[2].Trim()
        Write-Host "  ▸ $cmd" -ForegroundColor $Script:Config.Colors.Accent
        return
    }

    # Classic step
    if ($Line -match '^Step\s+(\d+)/(\d+)\s*:\s*(.+)$') {
        Write-Host "  ▸ $($matches[3])" -ForegroundColor $Script:Config.Colors.Accent
        return
    }

    # Package noise
    if ($Line -match '^(Get|Hit|Ign|Fetch|Reading|Download|Selecting|Preparing|Unpacking|Setting up)') {
        Write-Host "    $Line" -ForegroundColor $Script:Config.Colors.Dim
        return
    }

    # Errors - use word boundaries to avoid matching liberror etc.
    if ($Line -match '\b(error|failed|ERROR|FAILED)\b') {
        Write-Host "  $($Script:Config.Theme.Cross) $Line" -ForegroundColor $Script:Config.Colors.Error
        return
    }

    # Warnings
    if ($Line -match 'warning|WARNING|warn') {
        Write-Host "  [!] $Line" -ForegroundColor $Script:Config.Colors.Warn
        return
    }

    # Success
    if ($Line -match 'successfully tagged|Successfully built|built') {
        Write-Host "  $($Script:Config.Theme.Check) $Line" -ForegroundColor $Script:Config.Colors.Success
        return
    }

    Write-Host "    $Line" -ForegroundColor $Script:Config.Colors.Dim
}

# =============================================================================
# Docker build executor
# =============================================================================
function Invoke-DockerBuild {
    param(
        [string]$Platform,
        [string]$Dockerfile,
        [string]$ImageName,
        [string]$BuildContext,
        [bool]$NoCache = $false,
        [hashtable]$BuildArgs = @{}
    )

    Show-Stage "BUILD" "Creating image: $ImageName"

    $dfEsc = $Dockerfile.Replace('\', '/')
    $ctxEsc = $BuildContext.Replace('\', '/')
    $cacheArg = if ($NoCache) { "--no-cache" } else { "" }

    # Add build args
    $argList = [System.Collections.Generic.List[string]]::new()
    foreach ($kvp in $BuildArgs.GetEnumerator()) {
        $argList.Add("--build-arg $($kvp.Key)=$($kvp.Value)")
    }
    $buildArgsStr = $argList -join " "

    $dockerCmd = "build --progress=plain --platform=$Platform $cacheArg $buildArgsStr " +
    "-f ""$dfEsc"" -t $ImageName ""$ctxEsc"""

    $psi = [System.Diagnostics.ProcessStartInfo]@{
        FileName               = "docker"
        Arguments              = $dockerCmd
        RedirectStandardOutput = $true
        RedirectStandardError  = $true
        UseShellExecute        = $false
        CreateNoWindow         = $true
    }

    $proc = [System.Diagnostics.Process]@{ StartInfo = $psi }
    $queue = [System.Collections.Concurrent.ConcurrentQueue[string]]::new()

    Write-Host ""
    $proc.Start() | Out-Null

    $readerScript = {
        param([System.IO.StreamReader]$stream,
            [System.Collections.Concurrent.ConcurrentQueue[string]]$q)
        while (($line = $stream.ReadLine()) -ne $null) { $q.Enqueue($line) }
    }

    $rsOut = [PowerShell]::Create()
    $null = $rsOut.AddScript($readerScript).AddArgument($proc.StandardOutput).AddArgument($queue)
    $arOut = $rsOut.BeginInvoke()

    $rsErr = [PowerShell]::Create()
    $null = $rsErr.AddScript($readerScript).AddArgument($proc.StandardError).AddArgument($queue)
    $arErr = $rsErr.BeginInvoke()

    $item = [string]::Empty
    while (-not $proc.HasExited) {
        while ($queue.TryDequeue([ref]$item)) { Format-DockerBuildLine $item }
        Start-Sleep -Milliseconds 50
    }

    $rsOut.EndInvoke($arOut); $rsOut.Dispose()
    $rsErr.EndInvoke($arErr); $rsErr.Dispose()
    $proc.WaitForExit()
    while ($queue.TryDequeue([ref]$item)) { Format-DockerBuildLine $item }

    Write-Host ""
    if ($proc.ExitCode -eq 0) {
        Show-Success "Image built successfully"
        return $true
    }
    Show-Error "Build failed (exit $($proc.ExitCode))"
    return $false
}

# =============================================================================
# Help
# =============================================================================
if ($Help) {
    Write-Host @"
Usage: .\scripts\build_helpers\docker_start.ps1 [options]

Options:
  -Arch <amd64|arm64>  Target architecture (default: amd64)
  -FastBuild           Skip image cleanup, reuse existing if available
  -Interactive         Enter interactive shell (default behavior)
  -Verify              Run CI build verification instead of shell
  -BuildProject        Build image + run full clean build
  -BuildProjectFast    Build image + run fast build (no clean)
  -RunProjectTest      Build image + run tests
  -StayOnError         Stay in container on CI failure for debugging
  -NoLog               Disable file logging (default: log to file)
  -Help                Show this help message

Architecture Details:
  amd64    - Native x86_64 build, uses x86_64 Qt
  arm64    - ARM64 native container, uses linux/arm64 platform (QEMU emulation on x86)

Examples:
  .\docker_start.ps1                      # Interactive shell
  .\docker_start.ps1 -Verify              # Run CI build
  .\docker_start.ps1 -Verify -StayOnError # Debug on error
  .\docker_start.ps1 -BuildProject        # Build image + full clean build
  .\docker_start.ps1 -BuildProjectFast    # Build image + fast build
  .\docker_start.ps1 -RunProjectTest      # Build image + run tests
  .\docker_start.ps1 -FastBuild           # Reuse existing image
  .\docker_start.ps1 -Arch arm64          # ARM64 build

Note:
  - arm64 uses native ARM64 containers via Docker's platform emulation
  - On x86_64 hosts, ARM64 uses QEMU emulation (slower but produces native ARM64 binaries)
"@
    exit 0
}

# =============================================================================
# Main flow
# =============================================================================
# Docker platform mapping
# - amd64: native, use linux/amd64
# - arm64: native ARM64 container, use linux/arm64 (QEMU emulation on x86)
$DockerPlatform = "linux/$Arch"

# Determine run mode
$RunVerify = $Verify.IsPresent -and -not $Interactive.IsPresent -and -not $BuildProject.IsPresent -and -not $BuildProjectFast.IsPresent -and -not $RunProjectTest.IsPresent
$RunBuildProject = $BuildProject.IsPresent
$RunBuildProjectFast = $BuildProjectFast.IsPresent
$RunProjectTest = $RunProjectTest.IsPresent

# Setup logging before any output
Initialize-Logging

Show-Header "$($Script:Config.ProjectName) Docker Build Environment"

Show-Stage "CONFIG" "Build parameters"
Show-Info "Architecture: $Arch"
Show-Info "Platform: $DockerPlatform"
Show-Info "Image: $($Script:Config.ImageName)"
Show-Info "Fast build: $($FastBuild.IsPresent)"
$mode = if ($RunVerify) { 'CI Verify' }
        elseif ($RunBuildProject) { 'Full Clean Build' }
        elseif ($RunBuildProjectFast) { 'Fast Build' }
        elseif ($RunProjectTest) { 'Run Tests' }
        else { 'Interactive' }
Show-Info "Mode: $mode"

# =============================================================================
# Paths
# =============================================================================
$ScriptDir = $PSScriptRoot
$ProjectRoot = Split-Path -Parent (Split-Path -Parent $ScriptDir)
$DockerDir = Join-Path $ProjectRoot "scripts\docker"
$DockerfilePath = Join-Path $DockerDir "Dockerfile.build"

function ConvertTo-UnixPath([string]$p) {
    $p -replace '\\', '/' -replace '^([A-Za-z]):', '/$1'
}
$ProjectRootUnix = ConvertTo-UnixPath $ProjectRoot

Show-Info "Project root: $ProjectRoot"

# =============================================================================
# Check Docker
# =============================================================================
Show-Stage "CHECK" "Verifying Docker"

$dockerVer = docker --version 2>&1
if ($LASTEXITCODE -ne 0) {
    Show-Error "Docker not found. Install: https://www.docker.com/products/docker-desktop"
    exit 1
}
Show-Success "Docker: $dockerVer"

docker info 2>&1 | Out-Null
if ($LASTEXITCODE -ne 0) {
    Show-Error "Docker daemon not running. Start Docker Desktop."
    exit 1
}
Show-Success "Docker daemon is running"

# =============================================================================
# Clean and build image (default: always clean + rebuild)
# =============================================================================

# Get build args for Qt architecture
function Get-BuildArgs {
    $buildArgs = @{}
    if ($Interactive.IsPresent) { $buildArgs["INSTALL_DEPS"] = "0" }
    $qtArch = switch ($Arch) {
        "amd64" { "linux_gcc_64" }
        "arm64"  { "linux_gcc_arm64" }
        "armhf"  { "linux_gcc_arm64" }
        default  { "linux_gcc_64" }
    }
    $buildArgs["QT_ARCH"] = $qtArch
    return $buildArgs
}

# Build image with cache (used by -BuildProject and -BuildProjectFast)
function Build-ImageWithCache {
    Show-Stage "BUILD" "Building image with cache: $($Script:Config.ImageName)"
    $buildArgs = Get-BuildArgs
    $ok = Invoke-DockerBuild -Platform $DockerPlatform -Dockerfile $DockerfilePath `
        -ImageName $Script:Config.ImageName -BuildContext $ProjectRoot -NoCache $false -BuildArgs $buildArgs
    if (-not $ok) { exit 1 }
}

if ($FastBuild) {
    # Fast mode: only build if image doesn't exist
    $existing = docker images --filter "reference=$($Script:Config.ImageName)" --format "{{.ID}}" 2>&1
    if ($LASTEXITCODE -eq 0 -and $existing) {
        Show-Success "Using existing image (fast build mode)"
    }
    else {
        Show-Stage "BUILD" "Image not found, building..."
        $buildArgs = Get-BuildArgs
        $ok = Invoke-DockerBuild -Platform $DockerPlatform -Dockerfile $DockerfilePath `
            -ImageName $Script:Config.ImageName -BuildContext $ProjectRoot -NoCache $false -BuildArgs $buildArgs
        if (-not $ok) { exit 1 }
    }
}
elseif ($RunBuildProject -or $RunBuildProjectFast -or $RunProjectTest) {
    # BuildProject/Test modes: always build with cache
    Build-ImageWithCache
}
else {
    # Default mode: ALWAYS clean and rebuild
    Show-Stage "CLEAN" "Removing old image: $($Script:Config.ImageName)"
    $existing = docker images --filter "reference=$($Script:Config.ImageName)" --format "{{.ID}}" 2>&1
    if ($LASTEXITCODE -eq 0 -and $existing) {
        docker rmi "$($Script:Config.ImageName)" --force 2>&1 | Out-Null
        Show-Success "Old image removed"
    }
    else {
        Show-Info "No existing image to remove"
    }

    Show-Stage "BUILD" "Building fresh image: $($Script:Config.ImageName)"
    $buildArgs = Get-BuildArgs
    $ok = Invoke-DockerBuild -Platform $DockerPlatform -Dockerfile $DockerfilePath `
        -ImageName $Script:Config.ImageName -BuildContext $ProjectRoot -NoCache $true -BuildArgs $buildArgs
    if (-not $ok) { exit 1 }
}

if ($RunVerify) {
    Show-Stage "VERIFY" "Running CI build for $Arch"
    Write-Host ""

    if ($StayOnError) {
        Show-Info "Stay-on-error enabled: container will remain open on failure"
        Write-Host ""

        $ttyFlags = if (-not [Console]::IsInputRedirected) { @("-it") } else { @() }

        # Create a temporary script file to avoid quoting hell
        $tmpScript = [System.IO.Path]::GetTempFileName()
        @"
bash scripts/build_helpers/ci_build_entry.sh ci
exit_code=`$?
if [ `$exit_code -ne 0 ]; then
    echo ''
    echo '=== Build failed, staying in container for debugging ==='
    echo 'Type "exit" to leave the container'
    echo ''
    /bin/bash
fi
exit `$exit_code
"@ | Out-File -FilePath $tmpScript -Encoding utf8 -NoNewline

        try {
            & docker run $ttyFlags -i --rm --platform=$DockerPlatform `
                -v "${ProjectRootUnix}:/project" `
                -v "${tmpScript}:/tmp/debug_build.sh:ro" `
                -w /project `
                $Script:Config.ImageName bash /tmp/debug_build.sh

            $ec = $LASTEXITCODE
        }
        finally {
            Remove-Item $tmpScript -Force -ErrorAction SilentlyContinue
        }
        if ($ec -eq 0) {
            Write-Host ""
            Show-Success "CI build completed successfully"
        }
        else {
            Write-Host ""
            Show-Error "CI build failed (exit $ec)"
            Show-Info "Container session closed"
            exit $ec
        }
    }
    else {
        $psi = [System.Diagnostics.ProcessStartInfo]@{
            FileName               = "docker"
            Arguments              = "run --rm --platform=$DockerPlatform -v ${ProjectRootUnix}:/project -w /project $($Script:Config.ImageName) bash scripts/build_helpers/ci_build_entry.sh ci"
            RedirectStandardOutput = $true
            RedirectStandardError  = $true
            UseShellExecute        = $false
            CreateNoWindow         = $false
        }
        $proc = [System.Diagnostics.Process]@{ StartInfo = $psi }
        $queue = [System.Collections.Concurrent.ConcurrentQueue[string]]::new()

        $proc.Start() | Out-Null

        $readerScript = {
            param([System.IO.StreamReader]$stream,
                [System.Collections.Concurrent.ConcurrentQueue[string]]$q)
            while (($line = $stream.ReadLine()) -ne $null) { $q.Enqueue($line) }
        }

        $rsOut = [PowerShell]::Create()
        $null = $rsOut.AddScript($readerScript).AddArgument($proc.StandardOutput).AddArgument($queue)
        $arOut = $rsOut.BeginInvoke()

        $rsErr = [PowerShell]::Create()
        $null = $rsErr.AddScript($readerScript).AddArgument($proc.StandardError).AddArgument($queue)
        $arErr = $rsErr.BeginInvoke()

        $item = [string]::Empty
        while (-not $proc.HasExited) {
            while ($queue.TryDequeue([ref]$item)) { Write-Host "  $item" }
            Start-Sleep -Milliseconds 50
        }
        $rsOut.EndInvoke($arOut); $rsOut.Dispose()
        $rsErr.EndInvoke($arErr); $rsErr.Dispose()
        $proc.WaitForExit()
        while ($queue.TryDequeue([ref]$item)) { Write-Host "  $item" }

        if ($proc.ExitCode -ne 0) {
            Write-Host ""
            Show-Error "CI build failed (exit $($proc.ExitCode))"
            exit $proc.ExitCode
        }
        Write-Host ""
        Show-Success "CI build completed successfully"
    }
}
elseif ($RunBuildProject -or $RunBuildProjectFast) {
    $buildScript = if ($RunBuildProject) { "linux_develop_build.sh" } else { "linux_fast_develop_build.sh" }
    $buildDesc = if ($RunBuildProject) { "Full Clean Build" } else { "Fast Build (incremental)" }

    Show-Stage "BUILD" "Running $buildDesc in container"
    Write-Host ""

    $ttyFlags = if (-not [Console]::IsInputRedirected) { @("-it") } else { @() }

    # Determine config file based on architecture
    $configFile = switch ($Arch) {
        "amd64"  { "build_ci_config.ini" }
        "arm64"  { "build_ci_aarch64_config.ini" }
        "armhf"  { "build_ci_armhf_config.ini" }
        default  { "build_ci_config.ini" }
    }

    & docker run $ttyFlags --rm --platform=$DockerPlatform `
        -v "${ProjectRootUnix}:/project" -w /project `
        $Script:Config.ImageName bash scripts/build_helpers/$buildScript ci -c $configFile

    $ec = $LASTEXITCODE
    if ($ec -eq 0) {
        Write-Host ""
        Show-Success "Build completed successfully"
    }
    else {
        Write-Host ""
        Show-Error "Build failed (exit $ec)"
        exit $ec
    }
}
elseif ($RunProjectTest) {
    Show-Stage "TEST" "Running tests in container"
    Write-Host ""

    $ttyFlags = if (-not [Console]::IsInputRedirected) { @("-it") } else { @() }

    # Determine config file based on architecture
    $configFile = switch ($Arch) {
        "amd64"  { "build_ci_config.ini" }
        "arm64"  { "build_ci_aarch64_config.ini" }
        "armhf"  { "build_ci_armhf_config.ini" }
        default  { "build_ci_config.ini" }
    }

    & docker run $ttyFlags --rm --platform=$DockerPlatform `
        -v "${ProjectRootUnix}:/project" -w /project `
        $Script:Config.ImageName bash scripts/build_helpers/linux_run_tests.sh ci -c $configFile

    $ec = $LASTEXITCODE
    if ($ec -eq 0) {
        Write-Host ""
        Show-Success "All tests passed"
    }
    else {
        Write-Host ""
        Show-Error "Tests failed (exit $ec)"
        exit $ec
    }
}
else {
    Show-Stage "RUN" "Starting interactive shell"
    Show-Info "Mount: $ProjectRootUnix → /project"
    Show-Info "Type 'exit' to leave the container"
    Write-Host ""
    Write-Host ("  " + $Script:Config.Theme.Bold * 56) -ForegroundColor $Script:Config.Colors.Dim
    Write-Host ""

    $ttyFlags = if (-not [Console]::IsInputRedirected) { @("-it") } else { @() }
    & docker run $ttyFlags --rm --platform $DockerPlatform `
        -v "${ProjectRootUnix}:/project" -w /project `
        $Script:Config.ImageName /bin/bash

    $ec = $LASTEXITCODE
    if ($ec -ne 0 -and $ec -ne 130) {
        Show-Warn "Container exited with code $ec"
    }
}

Show-Stage "DONE" "Session ended"
Write-Host ""

# Close logging
Close-Logging
