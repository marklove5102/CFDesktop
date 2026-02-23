#!/bin/bash
# This script runs CMake tests using CTest
# It reads the build directory from the specified config file

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

# Default config
CONFIG="${1:-develop}"

log "========================================" "INFO"
log "Running Tests (Config: $CONFIG)" "INFO"
log "========================================" "INFO"

# Get the script directory and project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# Script is in scripts/build_helpers/, so project root is two levels up
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

log "Project root: $PROJECT_ROOT" "INFO"
cd "$PROJECT_ROOT"

# Determine which config file to use
case "$CONFIG" in
    develop)
        CONFIG_FILE="build_develop_config.ini"
        ;;
    deploy)
        CONFIG_FILE="build_deploy_config.ini"
        ;;
    *)
        log "Unknown config: $CONFIG. Valid options are: develop, deploy" "ERROR"
        exit 1
        ;;
esac

CONFIG_FILE="$SCRIPT_DIR/$CONFIG_FILE"
log "Loading configuration from: $CONFIG_FILE" "INFO"

# Parse and evaluate configuration
eval "$(get_ini_config "$CONFIG_FILE")"
log "Configuration loaded successfully!" "SUCCESS"

# Get build directory from config
BUILD_DIR="$config_paths_build_dir"
BUILD_DIR="$PROJECT_ROOT/$BUILD_DIR/test"

log "Test directory: $BUILD_DIR" "INFO"
log "Command: ctest --test-dir $BUILD_DIR --output-on-failure" "INFO"

# Check if build directory exists
if [[ ! -d "$BUILD_DIR" ]]; then
    log "Build directory does not exist: $BUILD_DIR" "ERROR"
    log "Please run the build script first before running tests." "ERROR"
    exit 1
fi

# Run tests
log "========================================" "INFO"
log "Running tests..." "INFO"
log "========================================" "INFO"

if ctest --test-dir "$BUILD_DIR" --output-on-failure; then
    log "========================================" "INFO"
    log "All tests passed successfully!" "SUCCESS"
    log "========================================" "INFO"
    exit 0
else
    exit_code=$?
    log "========================================" "INFO"
    log "Some tests failed with exit code: $exit_code" "WARNING"
    log "========================================" "INFO"
    exit $exit_code
fi
