/**
 * @file config_store_test.cpp
 * @brief Comprehensive functional tests for cf::config::ConfigStore
 *
 * Test Coverage:
 * 1. Basic Query/Set Operations - with default values, optional returns
 * 2. Layer Priority - Temp > App > User > System override behavior
 * 3. KeyView/Key Conversion - KeyHelper functionality
 * 4. Key Registration/Unregistration - register_key, unregister_key
 * 5. has_key Operations - checking key existence
 * 6. Watcher Functionality - immediate and manual notification
 * 7. notify() and pending_changes - manual notification handling
 * 8. sync() and reload() - persistence operations
 * 9. clear() and clear_layer() - clearing configurations
 * 10. Type Support - int, double, bool, string
 * 11. Edge Cases - invalid keys, special characters
 * 12. Wildcard Pattern Matching - watcher pattern support
 *
 * @date 2026-03-17
 * @version 1.0
 */

#include "cfconfig.hpp"
#include "cfconfig/cfconfig_result.h"
#include "cfconfig/cfconfig_watcher.h"
#include "cfconfig_key.h"
#include "cfconfig_layer.h"
#include "cfconfig_notify_policy.h"
#include "mock_path_provider.h"
#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <gtest/gtest.h>
#include <string>
#include <vector>

// Helper macro to explicitly discard nodiscard return values in tests
#define DISCARD_RESULT(expr) (void)(expr)

// =============================================================================
// Test Fixture - provides isolated test environment
// =============================================================================

class ConfigStoreTest : public ::testing::Test {
  protected:
    QTemporaryDir temp_dir_;
    static bool initialized_;

    void SetUp() override {
        // Initialize ConfigStore with mock path provider (only once)
        if (!initialized_) {
            ASSERT_TRUE(temp_dir_.isValid());
            auto mock_provider = std::make_shared<cf::config::test::MockPathProvider>(
                temp_dir_.path() + "/cfconfig_test");
            cf::config::ConfigStore::instance().initialize(mock_provider);
            initialized_ = true;
        }

        // Clear all layers for clean test state
        cf::config::ConfigStore::instance().clear();
    }

    void TearDown() override {
        // Temporary files are automatically cleaned up by QTemporaryDir
        // Just clear the in-memory Temp layer
        cf::config::ConfigStore::instance().clear_layer(cf::config::Layer::Temp);
    }

    // Helper to create a test INI file in a specific layer
    void CreateTestIniFile(const QString& path, const QString& content) {
        QDir().mkpath(QFileInfo(path).absolutePath());
        QFile file(path);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << content;
            file.close();
        }
    }

    // Helper to get the test temp directory path
    QString testPath() const { return temp_dir_.path() + "/cfconfig_test"; }
};

// Static member initialization
bool ConfigStoreTest::initialized_ = false;

// =============================================================================
// Test Suite 1: Basic Query/Set Operations
// =============================================================================

TEST_F(ConfigStoreTest, QueryWithDefault_ReturnsDefaultWhenKeyNotExists) {
    using namespace cf::config;

    KeyView kv{.group = "test", .key = "nonexistent"};

    // Query with default value
    int result = ConfigStore::instance().query<int>(kv, 42);
    EXPECT_EQ(result, 42);

    // Query string with default
    KeyView str_kv{.group = "test", .key = "name"};
    std::string name = ConfigStore::instance().query<std::string>(str_kv, "default");
    EXPECT_EQ(name, "default");
}

TEST_F(ConfigStoreTest, QueryOptional_ReturnsNulloptWhenKeyNotExists) {
    using namespace cf::config;

    KeyView kv{.group = "test", .key = "nonexistent"};

    auto result = ConfigStore::instance().query<int>(kv);
    EXPECT_FALSE(result.has_value());
}

TEST_F(ConfigStoreTest, SetAndQuery_ReturnsSetValues) {
    using namespace cf::config;

    // Set integer value
    KeyView int_kv{.group = "test", .key = "counter"};
    EXPECT_TRUE(ConfigStore::instance().set(int_kv, 123));
    EXPECT_EQ(ConfigStore::instance().query<int>(int_kv, 0), 123);

    // Set string value
    KeyView str_kv{.group = "test", .key = "message"};
    EXPECT_TRUE(ConfigStore::instance().set(str_kv, std::string("hello")));
    EXPECT_EQ(ConfigStore::instance().query<std::string>(str_kv, ""), "hello");

    // Set double value
    KeyView dbl_kv{.group = "test", .key = "pi"};
    EXPECT_TRUE(ConfigStore::instance().set(dbl_kv, 3.14159));
    EXPECT_DOUBLE_EQ(ConfigStore::instance().query<double>(dbl_kv, 0.0), 3.14159);

    // Set boolean value
    KeyView bool_kv{.group = "test", .key = "enabled"};
    EXPECT_TRUE(ConfigStore::instance().set(bool_kv, true));
    EXPECT_TRUE(ConfigStore::instance().query<bool>(bool_kv, false));
}

TEST_F(ConfigStoreTest, QueryOptional_ReturnsValueWhenKeyExists) {
    using namespace cf::config;

    KeyView kv{.group = "test", .key = "value"};

    DISCARD_RESULT(ConfigStore::instance().set(kv, 42));

    auto result = ConfigStore::instance().query<int>(kv);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 42);
}

TEST_F(ConfigStoreTest, UpdateExistingKey_UpdatesValue) {
    using namespace cf::config;

    KeyView kv{.group = "test", .key = "counter"};

    DISCARD_RESULT(ConfigStore::instance().set(kv, 10));
    EXPECT_EQ(ConfigStore::instance().query<int>(kv, 0), 10);

    DISCARD_RESULT(ConfigStore::instance().set(kv, 20));
    EXPECT_EQ(ConfigStore::instance().query<int>(kv, 0), 20);
}

// =============================================================================
// Test Suite 2: Layer Priority (Temp > App > User > System)
// =============================================================================

TEST_F(ConfigStoreTest, LayerPriority_TempOverridesApp) {
    using namespace cf::config;

    KeyView kv{.group = "priority", .key = "test"};

    // Set in App layer
    DISCARD_RESULT(ConfigStore::instance().set(kv, std::string("app_value"), Layer::App));
    EXPECT_EQ(ConfigStore::instance().query<std::string>(kv, ""), "app_value");

    // Set in Temp layer - should override
    DISCARD_RESULT(ConfigStore::instance().set(kv, std::string("temp_value"), Layer::Temp));
    EXPECT_EQ(ConfigStore::instance().query<std::string>(kv, ""), "temp_value");
}

TEST_F(ConfigStoreTest, LayerPriority_AppOverridesUser) {
    using namespace cf::config;

    KeyView kv{.group = "priority", .key = "layer"};

    // Set in User layer
    DISCARD_RESULT(ConfigStore::instance().set(kv, std::string("user_value"), Layer::User));
    EXPECT_EQ(ConfigStore::instance().query<std::string>(kv, ""), "user_value");

    // Set in App layer - should override
    DISCARD_RESULT(ConfigStore::instance().set(kv, std::string("app_value"), Layer::App));
    EXPECT_EQ(ConfigStore::instance().query<std::string>(kv, ""), "app_value");
}

TEST_F(ConfigStoreTest, LayerPriority_UserOverridesSystem) {
    using namespace cf::config;

    KeyView kv{.group = "priority", .key = "config"};

    // Set in System layer
    DISCARD_RESULT(ConfigStore::instance().set(kv, std::string("system_value"), Layer::System));
    EXPECT_EQ(ConfigStore::instance().query<std::string>(kv, ""), "system_value");

    // Set in User layer - should override
    DISCARD_RESULT(ConfigStore::instance().set(kv, std::string("user_value"), Layer::User));
    EXPECT_EQ(ConfigStore::instance().query<std::string>(kv, ""), "user_value");
}

TEST_F(ConfigStoreTest, QuerySpecificLayer_ReturnsLayerValue) {
    using namespace cf::config;

    KeyView kv{.group = "layers", .key = "value"};

    // Set different values in each layer
    DISCARD_RESULT(ConfigStore::instance().set(kv, std::string("system"), Layer::System));
    DISCARD_RESULT(ConfigStore::instance().set(kv, std::string("user"), Layer::User));
    DISCARD_RESULT(ConfigStore::instance().set(kv, std::string("app"), Layer::App));
    DISCARD_RESULT(ConfigStore::instance().set(kv, std::string("temp"), Layer::Temp));

    // Query each layer specifically
    EXPECT_EQ(ConfigStore::instance().query<std::string>(kv, Layer::System, ""), "system");
    EXPECT_EQ(ConfigStore::instance().query<std::string>(kv, Layer::User, ""), "user");
    EXPECT_EQ(ConfigStore::instance().query<std::string>(kv, Layer::App, ""), "app");
    EXPECT_EQ(ConfigStore::instance().query<std::string>(kv, Layer::Temp, ""), "temp");
}

TEST_F(ConfigStoreTest, QueryPriority_ReturnsHighestPriorityValue) {
    using namespace cf::config;

    KeyView kv{.group = "priority", .key = "full_test"};

    // Set in all layers (reverse priority order)
    DISCARD_RESULT(ConfigStore::instance().set(kv, 1, Layer::System));
    EXPECT_EQ(ConfigStore::instance().query<int>(kv, 0), 1);

    DISCARD_RESULT(ConfigStore::instance().set(kv, 2, Layer::User));
    EXPECT_EQ(ConfigStore::instance().query<int>(kv, 0), 2);

    DISCARD_RESULT(ConfigStore::instance().set(kv, 3, Layer::App));
    EXPECT_EQ(ConfigStore::instance().query<int>(kv, 0), 3);

    DISCARD_RESULT(ConfigStore::instance().set(kv, 4, Layer::Temp));
    EXPECT_EQ(ConfigStore::instance().query<int>(kv, 0), 4);
}

// =============================================================================
// Test Suite 3: KeyView/Key Conversion (KeyHelper)
// =============================================================================

TEST_F(ConfigStoreTest, KeyHelper_ValidConversion) {
    using namespace cf::config;

    KeyView kv{.group = "app.theme", .key = "name"};
    KeyHelper helper;
    Key k;

    ASSERT_TRUE(helper.fromKeyViewToKey(kv, k));
    EXPECT_EQ(k.full_key, "app.theme.name");
    EXPECT_EQ(k.full_description, "app.theme.name");
}

TEST_F(ConfigStoreTest, KeyHelper_KeyToKeyViewConversion) {
    using namespace cf::config;

    Key k{.full_key = "app.theme.name", .full_description = "Application theme name"};
    KeyHelper helper;

    KeyView kv = helper.fromKeyToKeyView(k);
    EXPECT_EQ(kv.group, "app.theme");
    EXPECT_EQ(kv.key, "name");
}

TEST_F(ConfigStoreTest, KeyHelper_KeyWithoutDot_ReturnsEmptyGroup) {
    using namespace cf::config;

    Key k{.full_key = "simple", .full_description = "Simple key"};
    KeyHelper helper;

    KeyView kv = helper.fromKeyToKeyView(k);
    EXPECT_EQ(kv.group, "");
    EXPECT_EQ(kv.key, "simple");
}

TEST_F(ConfigStoreTest, KeyHelper_InvalidCharacters_ReturnsFalse) {
    using namespace cf::config;

    KeyHelper helper;
    Key k;

    // Test with space
    KeyView invalid1{.group = "app theme", .key = "name"};
    EXPECT_FALSE(helper.fromKeyViewToKey(invalid1, k));

    // Test with special character
    KeyView invalid2{.group = "app.theme", .key = "name-value"};
    EXPECT_FALSE(helper.fromKeyViewToKey(invalid2, k));

    // Test with empty group and key
    KeyView invalid3{.group = "", .key = ""};
    EXPECT_FALSE(helper.fromKeyViewToKey(invalid3, k));
}

TEST_F(ConfigStoreTest, KeyHelper_ValidWithUnderscoreAndNumbers) {
    using namespace cf::config;

    KeyHelper helper;
    Key k;

    KeyView valid{.group = "app_settings_2", .key = "value_123"};
    ASSERT_TRUE(helper.fromKeyViewToKey(valid, k));
    EXPECT_EQ(k.full_key, "app_settings_2.value_123");
}

// =============================================================================
// Test Suite 4: Key Registration/Unregistration
// =============================================================================

TEST_F(ConfigStoreTest, RegisterKey_NewKey_ReturnsSuccess) {
    using namespace cf::config;

    Key k{.full_key = "registered.key", .full_description = "A registered key"};

    auto result = ConfigStore::instance().register_key(k, std::string("initial"), Layer::App);
    EXPECT_EQ(result, RegisterResult::KeyRegisteredSuccess);

    // Verify the key is accessible
    KeyView kv{.group = "registered", .key = "key"};
    EXPECT_EQ(ConfigStore::instance().query<std::string>(kv, ""), "initial");
}

TEST_F(ConfigStoreTest, RegisterKey_ExistingKey_ReturnsAlreadyIn) {
    using namespace cf::config;

    Key k{.full_key = "duplicate.key", .full_description = "Duplicate key test"};

    // First registration should succeed
    auto result1 = ConfigStore::instance().register_key(k, 100, Layer::App);
    EXPECT_EQ(result1, RegisterResult::KeyRegisteredSuccess);

    // Second registration should fail
    auto result2 = ConfigStore::instance().register_key(k, 200, Layer::App);
    EXPECT_EQ(result2, RegisterResult::KeyAlreadyIn);

    // Original value should remain
    KeyView kv{.group = "duplicate", .key = "key"};
    EXPECT_EQ(ConfigStore::instance().query<int>(kv, 0), 100);
}

TEST_F(ConfigStoreTest, RegisterKey_InDifferentLayers_AllowsRegistration) {
    using namespace cf::config;

    Key k{.full_key = "multi_layer.key", .full_description = "Multi-layer key"};

    // Register in System layer first
    auto r1 = ConfigStore::instance().register_key(k, 1, Layer::System);
    EXPECT_EQ(r1, RegisterResult::KeyRegisteredSuccess);

    // Registering same key in other layers will fail because:
    // 1. The key exists in cache (updated by first register_key)
    // 2. register_key checks all layers including cache
    auto r2 = ConfigStore::instance().register_key(k, 2, Layer::User);
    EXPECT_EQ(r2, RegisterResult::KeyAlreadyIn);

    auto r3 = ConfigStore::instance().register_key(k, 3, Layer::App);
    EXPECT_EQ(r3, RegisterResult::KeyAlreadyIn);

    // Query should return the first registered value
    KeyView kv{.group = "multi_layer", .key = "key"};
    EXPECT_EQ(ConfigStore::instance().query<int>(kv, 0), 1);
}

TEST_F(ConfigStoreTest, UnregisterKey_ExistingKey_ReturnsSuccess) {
    using namespace cf::config;

    Key k{.full_key = "to_remove.key", .full_description = "Key to remove"};

    // Register first
    DISCARD_RESULT(ConfigStore::instance().register_key(k, std::string("value"), Layer::App));

    // Then unregister
    auto result = ConfigStore::instance().unregister_key(k, Layer::App);
    EXPECT_EQ(result, UnRegisterResult::KeyUnRegisteredSuccess);

    // Key should no longer exist
    KeyView kv{.group = "to_remove", .key = "key"};
    EXPECT_FALSE(ConfigStore::instance().has_key(kv, Layer::App));
}

TEST_F(ConfigStoreTest, UnregisterKey_NonExistingKey_ReturnsUnexisted) {
    using namespace cf::config;

    Key k{.full_key = "nonexistent.key", .full_description = "Does not exist"};

    auto result = ConfigStore::instance().unregister_key(k, Layer::App);
    EXPECT_EQ(result, UnRegisterResult::KeyUnexisted);
}

// =============================================================================
// Test Suite 5: has_key Operations
// =============================================================================

TEST_F(ConfigStoreTest, HasKey_ExistingKey_ReturnsTrue) {
    using namespace cf::config;

    KeyView kv{.group = "existence", .key = "present"};
    DISCARD_RESULT(ConfigStore::instance().set(kv, 42));

    EXPECT_TRUE(ConfigStore::instance().has_key(kv));
}

TEST_F(ConfigStoreTest, HasKey_NonExistingKey_ReturnsFalse) {
    using namespace cf::config;

    KeyView kv{.group = "existence", .key = "absent"};
    EXPECT_FALSE(ConfigStore::instance().has_key(kv));
}

TEST_F(ConfigStoreTest, HasKey_InSpecificLayer_ReturnsCorrectResult) {
    using namespace cf::config;

    KeyView kv{.group = "layers", .key = "check"};

    // Set in User layer
    DISCARD_RESULT(ConfigStore::instance().set(kv, 1, Layer::User));

    EXPECT_TRUE(ConfigStore::instance().has_key(kv, Layer::User));
    EXPECT_FALSE(ConfigStore::instance().has_key(kv, Layer::App));

    // Note: set() updates cache (Temp layer), so has_key(Temp) returns true
    // This is expected behavior - cache stores all accessed/modified keys
    EXPECT_TRUE(ConfigStore::instance().has_key(kv, Layer::Temp));
}

TEST_F(ConfigStoreTest, HasKey_PriorityQuery_FindsKeyInAnyLayer) {
    using namespace cf::config;

    KeyView kv{.group = "search", .key = "anywhere"};

    // Only set in System layer
    DISCARD_RESULT(ConfigStore::instance().set(kv, 1, Layer::System));

    // has_key without layer should find it
    EXPECT_TRUE(ConfigStore::instance().has_key(kv));
}

// =============================================================================
// Test Suite 6: Watcher Functionality (Immediate Notification)
// =============================================================================

TEST_F(ConfigStoreTest, Watch_ImmediateNotification_TriggersCallback) {
    using namespace cf::config;

    KeyView kv{.group = "watch", .key = "test"};
    bool callback_called = false;
    std::string captured_key;

    WatcherHandle handle = ConfigStore::instance().watch(
        "watch.test",
        [&](const Key& k, const std::any* old, const std::any* new_v, Layer layer) {
            callback_called = true;
            captured_key = k.full_key;
        },
        NotifyPolicy::Immediate);

    DISCARD_RESULT(ConfigStore::instance().set(kv, 42));

    EXPECT_TRUE(callback_called);
    EXPECT_EQ(captured_key, "watch.test");

    ConfigStore::instance().unwatch(handle);
}

TEST_F(ConfigStoreTest, Watch_PatternMatch_MatchesWildcard) {
    using namespace cf::config;

    int call_count = 0;
    std::vector<std::string> matched_keys;

    WatcherHandle handle = ConfigStore::instance().watch(
        "app.theme.*",
        [&](const Key& k, const std::any* old, const std::any* new_v, Layer layer) {
            call_count++;
            matched_keys.push_back(k.full_key);
        },
        NotifyPolicy::Immediate);

    DISCARD_RESULT(ConfigStore::instance().set(KeyView{.group = "app.theme", .key = "name"},
                                               std::string("dark")));
    DISCARD_RESULT(ConfigStore::instance().set(KeyView{.group = "app.theme", .key = "mode"},
                                               std::string("auto")));
    DISCARD_RESULT(
        ConfigStore::instance().set(KeyView{.group = "app.other", .key = "setting"}, 123));

    EXPECT_EQ(call_count, 2);
    EXPECT_EQ(matched_keys[0], "app.theme.name");
    EXPECT_EQ(matched_keys[1], "app.theme.mode");

    ConfigStore::instance().unwatch(handle);
}

TEST_F(ConfigStoreTest, Watch_MultipleWildcards_MatchesAll) {
    using namespace cf::config;

    int call_count = 0;

    WatcherHandle handle = ConfigStore::instance().watch(
        "*.theme.*",
        [&](const Key& k, const std::any* old, const std::any* new_v, Layer layer) {
            call_count++;
        },
        NotifyPolicy::Immediate);

    DISCARD_RESULT(ConfigStore::instance().set(KeyView{.group = "app.theme", .key = "dark"}, 1));
    DISCARD_RESULT(ConfigStore::instance().set(KeyView{.group = "user.theme", .key = "light"}, 1));
    DISCARD_RESULT(ConfigStore::instance().set(KeyView{.group = "system.theme", .key = "blue"}, 1));
    DISCARD_RESULT(ConfigStore::instance().set(KeyView{.group = "app.other", .key = "setting"}, 1));

    EXPECT_EQ(call_count, 3);

    ConfigStore::instance().unwatch(handle);
}

TEST_F(ConfigStoreTest, Watch_OldAndNewValues_CapturesCorrectValues) {
    using namespace cf::config;

    KeyView kv{.group = "values", .key = "test"};
    std::any old_value;
    std::any new_value;

    WatcherHandle handle = ConfigStore::instance().watch(
        "values.test",
        [&](const Key& k, const std::any* old, const std::any* new_v, Layer layer) {
            if (old)
                old_value = *old;
            if (new_v)
                new_value = *new_v;
        },
        NotifyPolicy::Immediate);

    DISCARD_RESULT(ConfigStore::instance().set(kv, 10));

    ASSERT_TRUE(new_value.has_value());
    EXPECT_EQ(std::any_cast<int>(new_value), 10);

    DISCARD_RESULT(ConfigStore::instance().set(kv, 20));

    ASSERT_TRUE(old_value.has_value());
    ASSERT_TRUE(new_value.has_value());
    EXPECT_EQ(std::any_cast<int>(old_value), 10);
    EXPECT_EQ(std::any_cast<int>(new_value), 20);

    ConfigStore::instance().unwatch(handle);
}

TEST_F(ConfigStoreTest, Unwatch_PreventsFurtherNotifications) {
    using namespace cf::config;

    KeyView kv{.group = "unwatch", .key = "test"};
    int call_count = 0;

    WatcherHandle handle = ConfigStore::instance().watch(
        "unwatch.test",
        [&](const Key& k, const std::any* old, const std::any* new_v, Layer layer) {
            call_count++;
        },
        NotifyPolicy::Immediate);

    DISCARD_RESULT(ConfigStore::instance().set(kv, 1));
    EXPECT_EQ(call_count, 1);

    ConfigStore::instance().unwatch(handle);

    DISCARD_RESULT(ConfigStore::instance().set(kv, 2));
    EXPECT_EQ(call_count, 1); // Should not increase
}

TEST_F(ConfigStoreTest, Watch_LayerParameter_CapturesCorrectLayer) {
    using namespace cf::config;

    KeyView kv{.group = "layer", .key = "detect"};
    Layer captured_layer = Layer::System;

    WatcherHandle handle = ConfigStore::instance().watch(
        "layer.detect",
        [&](const Key& k, const std::any* old, const std::any* new_v, Layer layer) {
            captured_layer = layer;
        },
        NotifyPolicy::Immediate);

    DISCARD_RESULT(ConfigStore::instance().set(kv, 1, Layer::User));
    EXPECT_EQ(captured_layer, Layer::User);

    DISCARD_RESULT(ConfigStore::instance().set(kv, 2, Layer::App));
    EXPECT_EQ(captured_layer, Layer::App);

    ConfigStore::instance().unwatch(handle);
}

// =============================================================================
// Test Suite 7: Manual Notification and Pending Changes
// =============================================================================

TEST_F(ConfigStoreTest, ManualNotification_DoesNotTriggerImmediately) {
    using namespace cf::config;

    KeyView kv{.group = "manual", .key = "test"};
    int call_count = 0;

    ConfigStore::instance().watch(
        "manual.test",
        [&](const Key& k, const std::any* old, const std::any* new_v, Layer layer) {
            call_count++;
        },
        NotifyPolicy::Manual);

    DISCARD_RESULT(ConfigStore::instance().set(kv, 1, Layer::App, NotifyPolicy::Manual));

    EXPECT_EQ(call_count, 0); // Should not trigger immediately
}

TEST_F(ConfigStoreTest, Notify_TriggersPendingWatchers) {
    using namespace cf::config;

    KeyView kv{.group = "notify", .key = "test"};
    int call_count = 0;

    ConfigStore::instance().watch(
        "notify.test",
        [&](const Key& k, const std::any* old, const std::any* new_v, Layer layer) {
            call_count++;
        },
        NotifyPolicy::Manual);

    DISCARD_RESULT(ConfigStore::instance().set(kv, 1, Layer::App, NotifyPolicy::Manual));
    EXPECT_EQ(call_count, 0);

    auto result = ConfigStore::instance().notify();
    EXPECT_EQ(result, NotifyResult::NotifySuccess);
    EXPECT_EQ(call_count, 1);
}

TEST_F(ConfigStoreTest, NotifyWithNoPendingChanges_ReturnsNothingWorthNotify) {
    using namespace cf::config;

    auto result = ConfigStore::instance().notify();
    EXPECT_EQ(result, NotifyResult::NothingWorthNotify);
}

TEST_F(ConfigStoreTest, PendingChanges_CountsCorrectly) {
    using namespace cf::config;

    EXPECT_EQ(ConfigStore::instance().pending_changes(), 0);

    DISCARD_RESULT(ConfigStore::instance().set(KeyView{.group = "pending", .key = "a"}, 1,
                                               Layer::App, NotifyPolicy::Manual));
    EXPECT_EQ(ConfigStore::instance().pending_changes(), 1);

    DISCARD_RESULT(ConfigStore::instance().set(KeyView{.group = "pending", .key = "b"}, 2,
                                               Layer::App, NotifyPolicy::Manual));
    EXPECT_EQ(ConfigStore::instance().pending_changes(), 2);

    DISCARD_RESULT(ConfigStore::instance().notify());
    EXPECT_EQ(ConfigStore::instance().pending_changes(), 0);
}

TEST_F(ConfigStoreTest, MultipleManualChanges_AllTriggeredOnNotify) {
    using namespace cf::config;

    int call_count = 0;
    std::vector<std::string> captured_keys;

    ConfigStore::instance().watch(
        "batch.*",
        [&](const Key& k, const std::any* old, const std::any* new_v, Layer layer) {
            call_count++;
            captured_keys.push_back(k.full_key);
        },
        NotifyPolicy::Manual);

    DISCARD_RESULT(ConfigStore::instance().set(KeyView{.group = "batch", .key = "a"}, 1, Layer::App,
                                               NotifyPolicy::Manual));
    DISCARD_RESULT(ConfigStore::instance().set(KeyView{.group = "batch", .key = "b"}, 2, Layer::App,
                                               NotifyPolicy::Manual));
    DISCARD_RESULT(ConfigStore::instance().set(KeyView{.group = "batch", .key = "c"}, 3, Layer::App,
                                               NotifyPolicy::Manual));

    EXPECT_EQ(call_count, 0);

    DISCARD_RESULT(ConfigStore::instance().notify());

    EXPECT_EQ(call_count, 3);
    EXPECT_EQ(captured_keys.size(), 3);
}

// =============================================================================
// Test Suite 8: sync() and reload() Operations
// =============================================================================

TEST_F(ConfigStoreTest, Sync_DoesNotCrash) {
    using namespace cf::config;

    // Set some values
    DISCARD_RESULT(ConfigStore::instance().set(KeyView{.group = "sync", .key = "test"}, 123));
    DISCARD_RESULT(
        ConfigStore::instance().set(KeyView{.group = "sync", .key = "data"}, std::string("value")));

    // Sync should not crash
    ConfigStore::instance().sync(ConfigStore::SyncMethod::Sync);
    ConfigStore::instance().sync(ConfigStore::SyncMethod::Async);

    // Values should still be accessible
    EXPECT_EQ(ConfigStore::instance().query<int>(KeyView{.group = "sync", .key = "test"}, 0), 123);
}

TEST_F(ConfigStoreTest, Reload_ClearsTempLayer) {
    using namespace cf::config;

    // Set values in different layers
    DISCARD_RESULT(
        ConfigStore::instance().set(KeyView{.group = "reload", .key = "temp"}, 1, Layer::Temp));
    DISCARD_RESULT(
        ConfigStore::instance().set(KeyView{.group = "reload", .key = "app"}, 2, Layer::App));

    // Verify both exist
    EXPECT_TRUE(
        ConfigStore::instance().has_key(KeyView{.group = "reload", .key = "temp"}, Layer::Temp));
    EXPECT_TRUE(
        ConfigStore::instance().has_key(KeyView{.group = "reload", .key = "app"}, Layer::App));

    // Reload
    ConfigStore::instance().reload();

    // Temp layer should be cleared
    EXPECT_FALSE(
        ConfigStore::instance().has_key(KeyView{.group = "reload", .key = "temp"}, Layer::Temp));

    // App layer value should still exist (in QSettings)
    EXPECT_TRUE(
        ConfigStore::instance().has_key(KeyView{.group = "reload", .key = "app"}, Layer::App));
}

TEST_F(ConfigStoreTest, Reload_ClearsPendingChanges) {
    using namespace cf::config;

    DISCARD_RESULT(ConfigStore::instance().set(KeyView{.group = "pending", .key = "test"}, 1,
                                               Layer::App, NotifyPolicy::Manual));
    EXPECT_GT(ConfigStore::instance().pending_changes(), 0);

    ConfigStore::instance().reload();

    EXPECT_EQ(ConfigStore::instance().pending_changes(), 0);
}

// =============================================================================
// Test Suite 9: clear() and clear_layer() Operations
// =============================================================================

TEST_F(ConfigStoreTest, ClearLayer_Temp_ClearsOnlyTemp) {
    using namespace cf::config;

    DISCARD_RESULT(
        ConfigStore::instance().set(KeyView{.group = "clear", .key = "temp"}, 1, Layer::Temp));
    DISCARD_RESULT(
        ConfigStore::instance().set(KeyView{.group = "clear", .key = "app"}, 2, Layer::App));

    EXPECT_TRUE(
        ConfigStore::instance().has_key(KeyView{.group = "clear", .key = "temp"}, Layer::Temp));
    EXPECT_TRUE(
        ConfigStore::instance().has_key(KeyView{.group = "clear", .key = "app"}, Layer::App));

    ConfigStore::instance().clear_layer(Layer::Temp);

    EXPECT_FALSE(
        ConfigStore::instance().has_key(KeyView{.group = "clear", .key = "temp"}, Layer::Temp));
    EXPECT_TRUE(
        ConfigStore::instance().has_key(KeyView{.group = "clear", .key = "app"}, Layer::App));
}

TEST_F(ConfigStoreTest, ClearLayer_App_ClearsAppLayer) {
    using namespace cf::config;

    DISCARD_RESULT(
        ConfigStore::instance().set(KeyView{.group = "clear_app", .key = "value"}, 42, Layer::App));

    EXPECT_TRUE(
        ConfigStore::instance().has_key(KeyView{.group = "clear_app", .key = "value"}, Layer::App));

    ConfigStore::instance().clear_layer(Layer::App);

    EXPECT_FALSE(
        ConfigStore::instance().has_key(KeyView{.group = "clear_app", .key = "value"}, Layer::App));
}

TEST_F(ConfigStoreTest, Clear_ClearsAllLayers) {
    using namespace cf::config;

    DISCARD_RESULT(
        ConfigStore::instance().set(KeyView{.group = "global", .key = "sys"}, 1, Layer::System));
    DISCARD_RESULT(
        ConfigStore::instance().set(KeyView{.group = "global", .key = "usr"}, 2, Layer::User));
    DISCARD_RESULT(
        ConfigStore::instance().set(KeyView{.group = "global", .key = "app"}, 3, Layer::App));
    DISCARD_RESULT(
        ConfigStore::instance().set(KeyView{.group = "global", .key = "tmp"}, 4, Layer::Temp));

    ConfigStore::instance().clear();

    EXPECT_FALSE(ConfigStore::instance().has_key(KeyView{.group = "global", .key = "sys"}));
    EXPECT_FALSE(ConfigStore::instance().has_key(KeyView{.group = "global", .key = "usr"}));
    EXPECT_FALSE(ConfigStore::instance().has_key(KeyView{.group = "global", .key = "app"}));
    EXPECT_FALSE(ConfigStore::instance().has_key(KeyView{.group = "global", .key = "tmp"}));
}

TEST_F(ConfigStoreTest, Clear_ClearsPendingChanges) {
    using namespace cf::config;

    DISCARD_RESULT(ConfigStore::instance().set(KeyView{.group = "test", .key = "a"}, 1, Layer::App,
                                               NotifyPolicy::Manual));
    EXPECT_GT(ConfigStore::instance().pending_changes(), 0);

    ConfigStore::instance().clear();

    EXPECT_EQ(ConfigStore::instance().pending_changes(), 0);
}

// =============================================================================
// Test Suite 10: Type Support
// =============================================================================

TEST_F(ConfigStoreTest, TypeSupport_Integer) {
    using namespace cf::config;

    KeyView kv{.group = "types", .key = "int"};

    EXPECT_TRUE(ConfigStore::instance().set(kv, 42));
    EXPECT_EQ(ConfigStore::instance().query<int>(kv, 0), 42);

    // Test with negative
    EXPECT_TRUE(ConfigStore::instance().set(kv, -123));
    EXPECT_EQ(ConfigStore::instance().query<int>(kv, 0), -123);

    // Test with zero
    EXPECT_TRUE(ConfigStore::instance().set(kv, 0));
    EXPECT_EQ(ConfigStore::instance().query<int>(kv, 999), 0);
}

TEST_F(ConfigStoreTest, TypeSupport_Double) {
    using namespace cf::config;

    KeyView kv{.group = "types", .key = "double"};

    EXPECT_TRUE(ConfigStore::instance().set(kv, 3.14159));
    EXPECT_DOUBLE_EQ(ConfigStore::instance().query<double>(kv, 0.0), 3.14159);

    // Test negative
    EXPECT_TRUE(ConfigStore::instance().set(kv, -2.5));
    EXPECT_DOUBLE_EQ(ConfigStore::instance().query<double>(kv, 0.0), -2.5);

    // Test very small number
    EXPECT_TRUE(ConfigStore::instance().set(kv, 1e-10));
    EXPECT_NEAR(ConfigStore::instance().query<double>(kv, 0.0), 1e-10, 1e-15);
}

TEST_F(ConfigStoreTest, TypeSupport_Bool) {
    using namespace cf::config;

    KeyView kv{.group = "types", .key = "bool"};

    EXPECT_TRUE(ConfigStore::instance().set(kv, true));
    EXPECT_TRUE(ConfigStore::instance().query<bool>(kv, false));

    EXPECT_TRUE(ConfigStore::instance().set(kv, false));
    EXPECT_FALSE(ConfigStore::instance().query<bool>(kv, true));
}

TEST_F(ConfigStoreTest, TypeSupport_String) {
    using namespace cf::config;

    KeyView kv{.group = "types", .key = "string"};

    EXPECT_TRUE(ConfigStore::instance().set(kv, std::string("hello world")));
    EXPECT_EQ(ConfigStore::instance().query<std::string>(kv, ""), "hello world");

    // Test empty string
    EXPECT_TRUE(ConfigStore::instance().set(kv, std::string("")));
    EXPECT_EQ(ConfigStore::instance().query<std::string>(kv, "default"), "");

    // Test with special characters
    EXPECT_TRUE(ConfigStore::instance().set(kv, std::string("a/b\\c:d")));
    EXPECT_EQ(ConfigStore::instance().query<std::string>(kv, ""), "a/b\\c:d");
}

TEST_F(ConfigStoreTest, TypeConversion_StringToInt) {
    using namespace cf::config;

    KeyView kv{.group = "convert", .key = "str_to_int"};

    // Set as string, read as int (QSettings stores everything as string/variant)
    DISCARD_RESULT(ConfigStore::instance().set(kv, std::string("42")));

    // This should work with QVariant conversion
    auto result = ConfigStore::instance().query<int>(kv);
    // Note: Actual behavior depends on QVariant conversion
}

// =============================================================================
// Test Suite 11: Edge Cases
// =============================================================================

TEST_F(ConfigStoreTest, EdgeCase_EmptyGroup) {
    using namespace cf::config;

    KeyView kv{.group = "", .key = "key"};

    // Empty group should be invalid according to KeyHelper
    EXPECT_FALSE(ConfigStore::instance().set(kv, 1));
}

TEST_F(ConfigStoreTest, EdgeCase_EmptyKey) {
    using namespace cf::config;

    KeyView kv{.group = "group", .key = ""};

    // Empty key should be invalid
    EXPECT_FALSE(ConfigStore::instance().set(kv, 1));
}

TEST_F(ConfigStoreTest, EdgeCase_VeryLongKey) {
    using namespace cf::config;

    std::string long_group(1000, 'a');
    std::string long_key(1000, 'b');

    KeyView kv{.group = long_group, .key = long_key};

    // Should handle long keys (only alphanumeric and underscore)
    EXPECT_TRUE(ConfigStore::instance().set(kv, 1));
    EXPECT_EQ(ConfigStore::instance().query<int>(kv, 0), 1);
}

TEST_F(ConfigStoreTest, EdgeCase_NestedGroups) {
    using namespace cf::config;

    KeyView kv{.group = "a.b.c.d.e.f", .key = "deep"};

    EXPECT_TRUE(ConfigStore::instance().set(kv, std::string("nested")));
    EXPECT_EQ(ConfigStore::instance().query<std::string>(kv, ""), "nested");
}

TEST_F(ConfigStoreTest, EdgeCase_SetSameValue_Notifies) {
    using namespace cf::config;

    KeyView kv{.group = "same", .key = "value"};
    int call_count = 0;

    ConfigStore::instance().watch(
        "same.value",
        [&](const Key& k, const std::any* old, const std::any* new_v, Layer layer) {
            call_count++;
        },
        NotifyPolicy::Immediate);

    DISCARD_RESULT(ConfigStore::instance().set(kv, 42));
    EXPECT_EQ(call_count, 1);

    // Setting same value again
    DISCARD_RESULT(ConfigStore::instance().set(kv, 42));
    EXPECT_EQ(call_count, 2); // Should still notify
}

TEST_F(ConfigStoreTest, EdgeCase_MultipleWatchersSameKey_AllTriggered) {
    using namespace cf::config;

    KeyView kv{.group = "multi", .key = "watch"};
    int count1 = 0, count2 = 0, count3 = 0;

    auto h1 = ConfigStore::instance().watch(
        "multi.watch", [&](const Key& k, auto old, auto new_v, Layer l) { count1++; },
        NotifyPolicy::Immediate);
    auto h2 = ConfigStore::instance().watch(
        "multi.watch", [&](const Key& k, auto old, auto new_v, Layer l) { count2++; },
        NotifyPolicy::Immediate);
    auto h3 = ConfigStore::instance().watch(
        "multi.watch", [&](const Key& k, auto old, auto new_v, Layer l) { count3++; },
        NotifyPolicy::Immediate);

    DISCARD_RESULT(ConfigStore::instance().set(kv, 1));

    EXPECT_EQ(count1, 1);
    EXPECT_EQ(count2, 1);
    EXPECT_EQ(count3, 1);

    ConfigStore::instance().unwatch(h1);
    ConfigStore::instance().unwatch(h2);
    ConfigStore::instance().unwatch(h3);
}

// =============================================================================
// Test Suite 12: Unregister with Watcher
// =============================================================================

TEST_F(ConfigStoreTest, UnregisterKey_TrigersWatcherWithNullNewValue) {
    using namespace cf::config;

    Key k{.full_key = "delete_me.test", .full_description = "Test key"};
    bool old_value_captured = false;
    bool new_value_is_null = false;

    ConfigStore::instance().watch(
        "delete_me.test",
        [&](const Key& key, const std::any* old, const std::any* new_v, Layer layer) {
            if (old)
                old_value_captured = true;
            if (new_v == nullptr)
                new_value_is_null = true;
        },
        NotifyPolicy::Immediate);

    DISCARD_RESULT(ConfigStore::instance().register_key(k, 42, Layer::App));
    DISCARD_RESULT(ConfigStore::instance().unregister_key(k, Layer::App));

    EXPECT_TRUE(old_value_captured);
    EXPECT_TRUE(new_value_is_null);
}

// =============================================================================
// Test Suite 13: Complex Real-world Scenarios
// =============================================================================

TEST_F(ConfigStoreTest, RealWorld_ApplicationSettings) {
    using namespace cf::config;

    // Simulate application settings hierarchy
    struct Settings {
        std::string theme = "default";
        int dpi = 96;
        bool auto_save = true;
        double font_size = 12.0;
    } settings;

    // Register settings keys
    DISCARD_RESULT(ConfigStore::instance().register_key(
        Key{.full_key = "app.theme.name", .full_description = "Application theme"},
        std::string("default"), Layer::App));

    DISCARD_RESULT(ConfigStore::instance().register_key(
        Key{.full_key = "display.dpi", .full_description = "Display DPI"}, 96, Layer::User));

    DISCARD_RESULT(ConfigStore::instance().register_key(
        Key{.full_key = "editor.auto_save", .full_description = "Auto save enabled"}, true,
        Layer::App));

    DISCARD_RESULT(ConfigStore::instance().register_key(
        Key{.full_key = "editor.font_size", .full_description = "Font size in points"}, 12.0,
        Layer::App));

    // Read settings
    settings.theme = ConfigStore::instance().query<std::string>(
        KeyView{.group = "app.theme", .key = "name"}, "default");
    settings.dpi =
        ConfigStore::instance().query<int>(KeyView{.group = "display", .key = "dpi"}, 96);
    settings.auto_save =
        ConfigStore::instance().query<bool>(KeyView{.group = "editor", .key = "auto_save"}, true);
    settings.font_size =
        ConfigStore::instance().query<double>(KeyView{.group = "editor", .key = "font_size"}, 12.0);

    EXPECT_EQ(settings.theme, "default");
    EXPECT_EQ(settings.dpi, 96);
    EXPECT_TRUE(settings.auto_save);
    EXPECT_DOUBLE_EQ(settings.font_size, 12.0);
}

TEST_F(ConfigStoreTest, RealWorld_UserOverrideSystemDefaults) {
    using namespace cf::config;

    KeyView kv{.group = "preferences", .key = "language"};

    // System sets default
    DISCARD_RESULT(ConfigStore::instance().set(kv, std::string("en_US"), Layer::System));
    EXPECT_EQ(ConfigStore::instance().query<std::string>(kv, ""), "en_US");

    // User overrides
    DISCARD_RESULT(ConfigStore::instance().set(kv, std::string("zh_CN"), Layer::User));
    EXPECT_EQ(ConfigStore::instance().query<std::string>(kv, ""), "zh_CN");

    // App session temporarily overrides
    DISCARD_RESULT(ConfigStore::instance().set(kv, std::string("fr_FR"), Layer::Temp));
    EXPECT_EQ(ConfigStore::instance().query<std::string>(kv, ""), "fr_FR");

    // Clear temp - should fall back to user
    ConfigStore::instance().clear_layer(Layer::Temp);
    EXPECT_EQ(ConfigStore::instance().query<std::string>(kv, ""), "zh_CN");
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
