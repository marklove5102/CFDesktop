# Include the custom helper function from parent directory
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake")

# =============================================================================
# Layer 4: Material Behavior Layer Tests
# =============================================================================

# =============================================================================
# state_machine_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      state_machine_test
    SOURCE_FILE    state_machine_test.cpp
    LINK_LIBRARIES cf_ui_widget_material;cf_ui_application_support;cf_ui_components;cf_ui_core;GTest::gtest;GTest::gtest_main
    LABELS         "ui;components;layer4;state"
    LOG_MODULE     ui_components_tests
)

# =============================================================================
# ripple_helper_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      ripple_helper_test
    SOURCE_FILE    ripple_helper_test.cpp
    LINK_LIBRARIES cf_ui_widget_material;cf_ui_application_support;cf_ui_components;cf_ui_core;GTest::gtest;GTest::gtest_main
    LABELS         "ui;components;layer4;ripple"
    LOG_MODULE     ui_components_tests
)

# =============================================================================
# elevation_controller_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      elevation_controller_test
    SOURCE_FILE    elevation_controller_test.cpp
    LINK_LIBRARIES cf_ui_widget_material;cf_ui_application_support;cf_ui_components;cf_ui_core;GTest::gtest;GTest::gtest_main
    LABELS         "ui;components;layer4;elevation"
    LOG_MODULE     ui_components_tests
)

# =============================================================================
# focus_ring_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      focus_ring_test
    SOURCE_FILE    focus_ring_test.cpp
    LINK_LIBRARIES cf_ui_widget_material;cf_ui_application_support;cf_ui_components;cf_ui_core;GTest::gtest;GTest::gtest_main
    LABELS         "ui;components;layer4;focus"
    LOG_MODULE     ui_components_tests
)

# =============================================================================
# painter_layer_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      painter_layer_test
    SOURCE_FILE    painter_layer_test.cpp
    LINK_LIBRARIES cf_ui_widget_material;cf_ui_application_support;cf_ui_components;cf_ui_core;GTest::gtest;GTest::gtest_main
    LABELS         "ui;components;layer4;painter"
    LOG_MODULE     ui_components_tests
)

log_info("UI_Components_Tests" "Configured ui/components tests")
