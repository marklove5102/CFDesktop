#!/bin/bash
# This script cleans the build directory then calls the fast version to build
set -e

# Function to read INI configuration file
get_ini_config() {
    local filepath="$1"
    local current_section=""
    local config=""

    if [[ ! -f "$filepath" ]]; then
        echo "ERROR: Configuration file not found: $filepath" >&2
        exit 1
    fi

    while IFS= read -r line || [[ -n "$line" ]]; do
        # Trim whitespace
        line=$(echo "$line" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')

        # Skip empty lines and comments
        if [[ -z "$line" ]] || [[ "$line" =~ ^[#\;] ]]; then
            continue
        fi

        # Section header
        if [[ "$line" =~ ^\[([^\]]+)\]$ ]]; then
            current_section="${BASH_REMATCH[1]}"
            continue
        fi

        # Key=value pair
        if [[ "$line" =~ ^([^=]+)=(.*)$ ]]; then
            local key=$(echo "${BASH_REMATCH[1]}" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')
            local value=$(echo "${BASH_REMATCH[2]}" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')

            if [[ -n "$current_section" ]]; then
                config+="config_${current_section}_${key}=\"$value\""$'\n'
            fi
        fi
    done < "$filepath"

    echo "$config"
}

# Log function
log() {
    local level="$2"
    local message="$1"
    local timestamp=$(date "+%Y-%m-%d %H:%M:%S")
    local color=""

    case "$level" in
        INFO)
            color="\033[0;36m"    # Cyan
            ;;
        SUCCESS)
            color="\033[0;32m"    # Green
            ;;
        WARNING)
            color="\033[0;33m"    # Yellow
            ;;
        ERROR)
            color="\033[0;31m"    # Red
            ;;
        *)
            color="\033[0m"       # White/Default
            ;;
    esac

    echo -e "${color}[$timestamp] [$level] $message\033[0m"
}

log "========================================" "INFO"
log "Starting Linux Build Process (Full Clean + Build)" "INFO"
log "========================================" "INFO"

# Get the script directory and project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# Script is in scripts/build_helpers/, so project root is two levels up
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

log "Project root: $PROJECT_ROOT" "INFO"
log "Changing to project directory" "INFO"

cd "$PROJECT_ROOT"

# Load configuration from INI file
CONFIG_FILE="$SCRIPT_DIR/build_develop_config.ini"
log "Loading configuration from: $CONFIG_FILE" "INFO"

# Parse and evaluate configuration
eval "$(get_ini_config "$CONFIG_FILE")"
log "Configuration loaded successfully!" "SUCCESS"

# Extract configuration values
SOURCE_DIR="$config_paths_source"
BUILD_DIR="$config_paths_build_dir"

# Resolve source directory: if relative, make it relative to project root
if [[ "$SOURCE_DIR" = /* ]]; then
    # Already an absolute path
    RESOLVED_SOURCE_DIR="$SOURCE_DIR"
else
    # Relative path, resolve against project root
    RESOLVED_SOURCE_DIR="$(cd "$PROJECT_ROOT/$SOURCE_DIR" 2>/dev/null && pwd)" || "$PROJECT_ROOT/$SOURCE_DIR"
fi

log "Source directory: $SOURCE_DIR (resolved: $RESOLVED_SOURCE_DIR)" "INFO"
log "Build directory: $BUILD_DIR" "INFO"

# Step 1: Clean build directory
log "========================================" "INFO"
log "Step 1: Cleaning build directory" "INFO"
log "========================================" "INFO"

FULL_BUILD_PATH="$PROJECT_ROOT/$BUILD_DIR"

# Step 1: Clear the Build Directory
if [[ -d "$FULL_BUILD_PATH" ]]; then
    log "Removing existing build directory: $FULL_BUILD_PATH" "INFO"
    if rm -rf "$FULL_BUILD_PATH"; then
        log "Build directory cleaned successfully!" "SUCCESS"
    else
        log "Failed to clean build directory" "ERROR"
        exit 1
    fi
else
    log "Build directory does not exist, skipping clean step" "INFO"
fi

# Step 2: Call the fast build script
log "========================================" "INFO"
log "Step 2: Calling fast build script" "INFO"
log "========================================" "INFO"

FAST_BUILD_SCRIPT="$SCRIPT_DIR/linux_fast_develop_build.sh"
log "Executing: $FAST_BUILD_SCRIPT" "INFO"

if bash "$FAST_BUILD_SCRIPT"; then
    log "========================================" "INFO"
    log "Build process completed successfully!" "SUCCESS"
    log "========================================" "INFO"
else
    exit_code=$?
    log "Fast build script failed with exit code: $exit_code" "ERROR"
    exit $exit_code
fi

# Step 3: Run tests
log "========================================" "INFO"
log "Step 3: Running tests" "INFO"
log "========================================" "INFO"

TEST_SCRIPT="$SCRIPT_DIR/linux_run_tests.sh"
log "Executing: $TEST_SCRIPT develop" "INFO"

if bash "$TEST_SCRIPT" develop; then
    log "========================================" "INFO"
    log "All tests passed successfully!" "SUCCESS"
    log "========================================" "INFO"
else
    exit_code=$?
    log "========================================" "INFO"
    log "Some tests failed with exit code: $exit_code" "WARNING"
    log "========================================" "INFO"
    # Don't exit on test failure, just warn
fi
