#!/bin/bash
# This script configures and builds the project (FAST version - no cleaning)
# It calls the configure script first, then builds
# Usage: ./linux_fast_deploy_build.sh [-c|--config <config_file>]
set -e

# Default values
CONFIG_MODE="deploy"
CONFIG_FILE=""

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -c|--config)
            CONFIG_FILE="$2"
            shift 2
            ;;
        develop|deploy|ci)
            CONFIG_MODE="$1"
            shift
            ;;
        *)
            echo "ERROR: Unknown argument '$1'" >&2
            echo "Usage: $0 [develop|deploy|ci] [-c|--config <config_file>]" >&2
            exit 1
            ;;
    esac
done

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
log "Starting Linux FAST Build Process (Deploy)" "INFO"
log "========================================" "INFO"

# Get the script directory and project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

log "Project root: $PROJECT_ROOT" "INFO"
cd "$PROJECT_ROOT"

# Step 1: Call the configure script
log "========================================" "INFO"
log "Step 1: Configuring with CMake" "INFO"
log "========================================" "INFO"

CONFIGURE_SCRIPT="$SCRIPT_DIR/linux_configure.sh"
CONFIGURE_ARGS=("$CONFIG_MODE")
if [[ -n "$CONFIG_FILE" ]]; then
    CONFIGURE_ARGS+=("-c" "$CONFIG_FILE")
fi
log "Executing: $CONFIGURE_SCRIPT ${CONFIGURE_ARGS[*]}" "INFO"

if bash "$CONFIGURE_SCRIPT" "${CONFIGURE_ARGS[@]}"; then
    log "Configuration completed successfully!" "SUCCESS"
else
    exit_code=$?
    log "Configure script failed with exit code: $exit_code" "ERROR"
    exit $exit_code
fi

# Step 2: Load config for build
# Determine config file if not specified
if [[ -z "$CONFIG_FILE" ]]; then
    if [[ "$CONFIG_MODE" == "deploy" ]]; then
        CONFIG_FILE="$SCRIPT_DIR/build_deploy_config.ini"
    elif [[ "$CONFIG_MODE" == "ci" ]]; then
        CONFIG_FILE="$SCRIPT_DIR/build_ci_config.ini"
    else
        CONFIG_FILE="$SCRIPT_DIR/build_develop_config.ini"
    fi
fi

# Resolve relative path
if [[ "$CONFIG_FILE" != /* ]] && [[ "$CONFIG_FILE" != ~* ]]; then
    CONFIG_FILE="$SCRIPT_DIR/$CONFIG_FILE"
fi

eval "$(get_ini_config "$CONFIG_FILE")"
BUILD_DIR="$config_paths_build_dir"
JOBS="${config_options_jobs:-}"

# Step 3: Build with CMake
log "========================================" "INFO"
log "Step 2: Building project" "INFO"

BUILD_CMD=(cmake --build "$BUILD_DIR")
if [[ -n "$JOBS" ]]; then
    BUILD_CMD+=(--parallel "$JOBS")
    log "Command: cmake --build $BUILD_DIR --parallel $JOBS" "INFO"
else
    log "Command: cmake --build $BUILD_DIR" "INFO"
fi
log "========================================" "INFO"

# Run build and stream output in real-time
"${BUILD_CMD[@]}"
