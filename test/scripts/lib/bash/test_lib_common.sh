#!/usr/bin/env bash
# =============================================================================
# Unit Tests for lib_common.sh
# =============================================================================
#
# Usage:
#   bash-unit test_lib_common.sh
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
source "$LIB_DIR/lib_common.sh"

# =============================================================================
# 测试用例
# =============================================================================

test_log_function_outputs_timestamp() {
    local output
    output=$(log "test message" "INFO" 2>&1)
    assert_contains "$output" "["
    assert_contains "$output" "]"
}

test_log_function_outputs_level() {
    local output
    output=$(log "test message" "INFO" 2>&1)
    assert_contains "$output" "[INFO]"
}

test_log_function_outputs_message() {
    local output
    output=$(log "test message" "INFO" 2>&1)
    assert_contains "$output" "test message"
}

test_log_info_outputs_info_level() {
    local output
    output=$(log_info "test message" 2>&1)
    assert_contains "$output" "[INFO]"
}

test_log_success_outputs_success_level() {
    local output
    output=$(log_success "test message" 2>&1)
    assert_contains "$output" "[SUCCESS]"
}

test_log_warn_outputs_warning_level() {
    local output
    output=$(log_warn "test message" 2>&1)
    assert_contains "$output" "[WARNING]"
}

test_log_error_outputs_error_level() {
    local output
    output=$(log_error "test message" 2>&1)
    assert_contains "$output" "[ERROR]"
}

test_color_constants_are_defined() {
    assert_not_empty "$RED"
    assert_not_empty "$GREEN"
    assert_not_empty "$YELLOW"
    assert_not_empty "$CYAN"
    assert_not_empty "$GRAY"
    assert_not_empty "$NC"
}

# 如果直接运行此脚本，执行所有测试
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    bash-unit "$0"
fi
