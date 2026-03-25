# Config manager module unit tests using GoogleTest
include(add_gtest_executable)

log_info("config_manager_tests" "Configured config manager unit tests:")

# Common include directory for headers
set(CONFIG_MANAGER_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../desktop/base/config_manager/include")
set(BASE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../base/include")

# =============================================================================
# Config manager unit tests - config_store_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      config_store_test
    SOURCE_FILE    config_store_test.cpp
    LINK_LIBRARIES CFDesktop::config;Qt6::Core;GTest::gtest;GTest::gtest_main
    LABELS         "config_manager;unit;functional"
    LOG_MODULE     config_manager_tests
    INCLUDE_DIRS   ${CONFIG_MANAGER_INCLUDE_DIR};${BASE_INCLUDE_DIR}
)
