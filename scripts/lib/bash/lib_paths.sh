#!/bin/bash
# =============================================================================
# CFDesktop Scripts Library - Paths Module
# =============================================================================
#
# 提供路径解析功能
#
# 用法:
#   source "$(dirname "${BASH_SOURCE[0]}")/lib_paths.sh"
#   script_dir=$(get_script_dir)
#   project_root=$(get_project_root)
#
# =============================================================================

# =============================================================================
# 路径解析函数
# =============================================================================

# 获取脚本所在目录
# 返回: 当前脚本所在目录的绝对路径
get_script_dir() {
    cd "$(dirname "${BASH_SOURCE[1]}")" && pwd
}

# 获取项目根目录
# 假设脚本在 scripts/ 的子目录中
# 返回: 项目根目录的绝对路径
get_project_root() {
    local script_dir
    script_dir="$(get_script_dir)"
    cd "$script_dir/../.." && pwd
}

# 获取 scripts 目录
# 返回: scripts 目录的绝对路径
get_scripts_dir() {
    local project_root
    project_root="$(get_project_root)"
    echo "$project_root/scripts"
}

# 获取库目录
# 返回: lib 目录的绝对路径
get_lib_dir() {
    local scripts_dir
    scripts_dir="$(get_scripts_dir)"
    echo "$scripts_dir/lib"
}

# 检查路径是否存在
# 参数:
#   $1 - 要检查的路径
# 返回:
#   0 - 存在
#   1 - 不存在
path_exists() {
    [[ -e "$1" ]]
}

# 创建目录（如果不存在）
# 参数:
#   $1 - 要创建的目录路径
ensure_dir() {
    local dir="$1"
    if [[ ! -d "$dir" ]]; then
        mkdir -p "$dir"
    fi
}

# =============================================================================
# 环境变量设置（供 source 时使用）
# =============================================================================

# 设置脚本目录环境变量
export SCRIPT_DIR="${SCRIPT_DIR:-$(get_script_dir)}"

# 设置项目根目录环境变量
export PROJECT_ROOT="${PROJECT_ROOT:-$(get_project_root)}"

# 设置 scripts 目录环境变量
export SCRIPTS_DIR="${SCRIPTS_DIR:-$(get_scripts_dir)}"

# 设置库目录环境变量
export LIB_DIR="${LIB_DIR:-$(get_lib_dir)}"
