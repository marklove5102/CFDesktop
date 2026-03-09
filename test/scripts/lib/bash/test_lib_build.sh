#!/usr/bin/env bash
# =============================================================================
# Unit Tests for lib_build.sh
# =============================================================================
#
# Usage:
#   bash-unit test_lib_build.sh
#
# =============================================================================

# 测试框架依赖检查
if ! command -v bash-unit >/dev/null 2>&1; then
    echo "ERROR: bash-unit is not installed."
    echo "Install from: https://github.com/pgrange/bash-unit"
    exit 1
fi

# 加载被测试的模块
TEST_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LIB_DIR="$(cd "$TEST_DIR/../../../scripts/lib/bash" && pwd)"
source "$LIB_DIR/lib_build.sh"

# =============================================================================
# 测试用例
# =============================================================================

test_ensure_build_dir_creates_directory() {
    local temp_dir
    temp_dir=$(mktemp -d)
    local build_dir="$temp_dir/test_build"

    ensure_build_dir "$build_dir"

    [[ -d "$build_dir" ]]
    assert_equals 0 $?

    rm -rf "$temp_dir"
}

test_ensure_build_dir_does_not_fail_if_exists() {
    local temp_dir
    temp_dir=$(mktemp -d)

    ensure_build_dir "$temp_dir"
    assert_equals 0 $?

    rm -rf "$temp_dir"
}

test_get_parallel_job_count_returns_positive_number() {
    local result
    result=$(get_parallel_job_count)
    assert_not_empty "$result"

    # 应该是一个正整数
    [[ "$result" =~ ^[0-9]+$ ]]
    assert_equals 0 $?

    [[ "$result" -gt 0 ]]
    assert_equals 0 $?
}

test_has_cmake_cache_returns_false_for_nonexistent_dir() {
    has_cmake_cache "/nonexistent/build/dir"
    assert_not_equals 0 $?
}

test_has_cmake_cache_returns_false_for_dir_without_cache() {
    local temp_dir
    temp_dir=$(mktemp -d)

    has_cmake_cache "$temp_dir"
    assert_not_equals 0 $?

    rm -rf "$temp_dir"
}

test_has_cmake_cache_returns_true_for_dir_with_cache() {
    local temp_dir
    temp_dir=$(mktemp -d)
    touch "$temp_dir/CMakeCache.txt"

    has_cmake_cache "$temp_dir"
    assert_equals 0 $?

    rm -rf "$temp_dir"
}

test_build_timer_functions() {
    # 测试计时器函数不报错
    build_timer_start
    assert_equals 0 $?

    # 等待一小段时间
    sleep 0.1

    # 注意：build_timer_end 会输出，但我们只检查是否不报错
    build_timer_end 2>&1 >/dev/null
    assert_equals 0 $?
}

test_clean_build_dir_removes_directory() {
    local temp_dir
    temp_dir=$(mktemp -d)
    local build_dir="$temp_dir/test_build"
    mkdir -p "$build_dir"
    touch "$build_dir/test_file.txt"

    clean_build_dir "$build_dir" 2>&1 >/dev/null
    assert_equals 0 $?

    # 目录应该被删除
    [[ ! -d "$build_dir" ]]
    assert_equals 0 $?

    rm -rf "$temp_dir"
}

test_clean_build_dir_skips_nonexistent_directory() {
    local temp_dir
    temp_dir=$(mktemp -d)
    local build_dir="$temp_dir/nonexistent_build"

    clean_build_dir "$build_dir" 2>&1 >/dev/null
    assert_equals 0 $?

    rm -rf "$temp_dir"
}

# 如果直接运行此脚本，执行所有测试
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    bash-unit "$0"
fi
