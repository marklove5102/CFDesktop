#!/usr/bin/env bash
# =============================================================================
# CFDesktop Scripts Library - Test Runner
# =============================================================================
#
# 运行所有 Bash 库的单元测试
#
# 用法:
#   ./run_all_tests.sh
#   ./run_all_tests.sh --verbose
#
# =============================================================================

set -eo pipefail

# =============================================================================
# 颜色和输出函数
# =============================================================================

readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly YELLOW='\033[0;33m'
readonly CYAN='\033[0;36m'
readonly GRAY='\033[0;90m'
readonly NC='\033[0m'

# 获取脚本目录
TEST_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LIB_TEST_DIR="$TEST_DIR/lib/bash"
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0
FAILED_TEST_LIST=()

# 打印分隔线
print_separator() {
    local char="${1:-=}"
    local width="${2:-80}"
    local line=""
    for ((i=0; i<width; i++)); do
        line+="$char"
    done
    echo -e "${GRAY}${line}${NC}"
}

# 打印头部
print_header() {
    echo ""
    print_separator "="
    echo -e "${CYAN}$1${NC}"
    print_separator "="
    echo ""
}

# 打印测试开始
print_test_start() {
    local test_name="$1"
    echo -e "${CYAN}▶ Running:${NC} $test_name"
    echo -e "${GRAY}$(printf ' %.0s' {1..80})${NC}"
}

# 打印测试通过
print_test_pass() {
    local test_name="$1"
    local duration="$2"
    echo -e "${GREEN}✓ PASSED:${NC} $test_name ${GRAY}(${duration})${NC}"
    echo ""
}

# 打印测试失败
print_test_fail() {
    local test_name="$1"
    local duration="$2"
    echo -e "${RED}✗ FAILED:${NC} $test_name ${GRAY}(${duration})${NC}"
    echo ""
}

# 打印警告
print_warning() {
    echo -e "${YELLOW}⚠ WARNING:${NC} $1"
}

# 格式化持续时间
format_duration() {
    local seconds="$1"
    if (( $(echo "$seconds < 1" | bc -l) )); then
        echo "$(echo "$seconds * 1000" | bc)ms"
    else
        echo "${seconds}s"
    fi
}

# =============================================================================
# 测试框架检测
# =============================================================================

check_bash_unit() {
    if ! command -v bash-unit >/dev/null 2>&1; then
        print_warning "bash-unit is not installed!"
        echo ""
        echo "To install bash-unit, run:"
        echo "  git clone https://github.com/pgrange/bash-unit.git /path/to/bash-unit"
        echo "  export PATH=\"\$PATH:/path/to/bash-unit\""
        echo ""
        return 1
    fi
    return 0
}

# =============================================================================
# 运行单个测试
# =============================================================================

run_single_test() {
    local test_file="$1"
    local test_name
    test_name="$(basename "$test_file")"

    print_test_start "$test_name"

    # 记录开始时间
    local start_time
    start_time=$(date +%s.%N)

    # 运行测试并捕获输出
    local output
    local exit_code=0

    # 运行测试，捕获输出到变量
    if output=$(bash-unit "$test_file" 2>&1); then
        exit_code=0
    else
        exit_code=$?
    fi

    # 计算持续时间
    local end_time
    end_time=$(date +%s.%N)
    local duration
    duration=$(echo "$end_time - $start_time" | bc)
    local formatted_duration
    formatted_duration=$(format_duration "$duration")

    # 显示详细输出（如果有）
    if [[ -n "$output" ]]; then
        echo -e "${GRAY}${output}${NC}" | sed 's/^/  /'
    fi

    # 更新统计
    ((TOTAL_TESTS++))

    if [[ $exit_code -eq 0 ]]; then
        ((PASSED_TESTS++))
        print_test_pass "$test_name" "$formatted_duration"
        return 0
    else
        ((FAILED_TESTS++))
        FAILED_TEST_LIST+=("$test_name")
        print_test_fail "$test_name" "$formatted_duration"
        return 1
    fi
}

# =============================================================================
# 运行所有测试
# =============================================================================

run_all_tests() {
    local verbose="${1:-false}"

    print_header "CFDesktop Scripts Library - Bash Unit Tests"

    # 检查测试框架
    if ! check_bash_unit; then
        exit 1
    fi

    echo -e "${GRAY}Test Directory:${NC} $LIB_TEST_DIR"
    echo -e "${GRAY}Test Framework:${NC} $(bash-unit --version 2>&1 | head -1)"
    echo -e "${GRAY}Timestamp:${NC} $(date '+%Y-%m-%d %H:%M:%S')"
    echo ""

    # 查找所有测试文件
    local test_files=()
    while IFS= read -r -d '' file; do
        test_files+=("$file")
    done < <(find "$LIB_TEST_DIR" -name "test_*.sh" -type f -print0 | sort -z)

    if [[ ${#test_files[@]} -eq 0 ]]; then
        print_warning "No test files found in $LIB_TEST_DIR"
        exit 1
    fi

    echo -e "${GRAY}Found ${#test_files[@]} test file(s)${NC}"
    echo ""

    # 运行每个测试
    for test_file in "${test_files[@]}"; do
        run_single_test "$test_file"
    done

    # 打印汇总
    print_summary
}

# =============================================================================
# 打印测试汇总
# =============================================================================

print_summary() {
    echo ""
    print_separator "="
    echo -e "${CYAN}Test Summary${NC}"
    print_separator "="
    echo ""

    # 测试统计
    echo -e "  ${GRAY}Total Tests:${NC}    $TOTAL_TESTS"
    echo -e "  ${GREEN}Passed:${NC}        $PASSED_TESTS"
    echo -e "  ${RED}Failed:${NC}        $FAILED_TESTS"

    # 通过率
    if [[ $TOTAL_TESTS -gt 0 ]]; then
        local pass_rate
        pass_rate=$(echo "scale=1; $PASSED_TESTS * 100 / $TOTAL_TESTS" | bc)
        local color="$GREEN"
        if (( $(echo "$pass_rate < 100" | bc -l) )); then
            color="$YELLOW"
        fi
        if (( $(echo "$pass_rate < 80" | bc -l) )); then
            color="$RED"
        fi
        echo -e "  ${color}Pass Rate:${NC}      ${pass_rate}%"
    fi

    echo ""

    # 失败的测试
    if [[ ${#FAILED_TEST_LIST[@]} -gt 0 ]]; then
        echo -e "${RED}Failed Tests:${NC}"
        for test_name in "${FAILED_TEST_LIST[@]}"; do
            echo -e "  ${RED}✗${NC} $test_name"
        done
        echo ""
    fi

    # 最终结果
    print_separator "="
    if [[ $FAILED_TESTS -eq 0 ]]; then
        echo -e "${GREEN}✓ All tests passed!${NC}"
        print_separator "="
        echo ""
        exit 0
    else
        echo -e "${RED}✗ Some tests failed!${NC}"
        print_separator "="
        echo ""
        exit 1
    fi
}

# =============================================================================
# 主函数
# =============================================================================

main() {
    local verbose=false

    # 解析参数
    while [[ $# -gt 0 ]]; do
        case $1 in
            -v|--verbose)
                verbose=true
                shift
                ;;
            -h|--help)
                echo "Usage: $0 [OPTIONS]"
                echo ""
                echo "Options:"
                echo "  -v, --verbose    Show verbose output"
                echo "  -h, --help       Show this help message"
                echo ""
                exit 0
                ;;
            *)
                echo "Unknown option: $1"
                echo "Use --help for usage information"
                exit 1
                ;;
        esac
    done

    run_all_tests "$verbose"
}

# 运行主函数
main "$@"
