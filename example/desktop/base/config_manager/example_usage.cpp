/**
 * @file example_usage.cpp
 * @brief ConfigStore usage examples for CFDesktop.
 *
 * Demonstrates:
 * 1. Basic query/set operations
 * 2. Layer priority (Temp > App > User > System)
 * 3. Key registration
 * 4. Watchers for change notification
 * 5. Type-safe queries with defaults
 *
 * @date 2026-03-17
 * @version 1.0
 */

#include "cfconfig.hpp"
#include "cflog.h"
#include <format>

void run_logger_init();

using namespace cf::config;
using namespace cf::log;

void print_separator(const std::string& title) {
    info("");
    info(std::format("========== {} ==========", title));
}

// ============================================================================
// Example 1: Basic Query/Set with Default Values
// ============================================================================

void example_basic_operations() {
    print_separator("Example 1: Basic Query/Set");

    // Query with default value (key doesn't exist yet)
    auto theme = ConfigStore::instance().query<std::string>(KeyView{.group = "app", .key = "theme"},
                                                            "default");
    info(std::format("Theme (with default): {}", theme));

    // Set a value
    bool ok =
        ConfigStore::instance().set(KeyView{.group = "app", .key = "theme"}, std::string("dark"));
    if (ok) {
        info("Set theme to 'dark' successfully");
    } else {
        warning("Failed to set theme");
    }

    // Query again (now it exists)
    theme = ConfigStore::instance().query<std::string>(KeyView{.group = "app", .key = "theme"},
                                                       "default");
    info(std::format("Theme (after set): {}", theme));

    // Optional query
    auto maybe_theme =
        ConfigStore::instance().query<std::string>(KeyView{.group = "app", .key = "theme"});
    if (maybe_theme) {
        info(std::format("Theme (optional): {}", *maybe_theme));
    }
}

// ============================================================================
// Example 2: Working with Different Types
// ============================================================================

void example_types() {
    print_separator("Example 2: Different Value Types");

    // Integer
    (void)ConfigStore::instance().set(KeyView{.group = "ui", .key = "width"}, 1920);
    int width = ConfigStore::instance().query<int>(KeyView{.group = "ui", .key = "width"}, 800);
    info(std::format("Window width: {}", width));

    // Double
    (void)ConfigStore::instance().set(KeyView{.group = "ui", .key = "scale"}, 1.5);
    double scale =
        ConfigStore::instance().query<double>(KeyView{.group = "ui", .key = "scale"}, 1.0);
    info(std::format("Display scale: {}", scale));

    // Boolean
    (void)ConfigStore::instance().set(KeyView{.group = "app", .key = "auto_save"}, true);
    bool auto_save =
        ConfigStore::instance().query<bool>(KeyView{.group = "app", .key = "auto_save"}, false);
    info(std::format("Auto-save: {}", auto_save ? "enabled" : "disabled"));
}

// ============================================================================
// Example 3: Layer Priority
// ============================================================================

void example_layer_priority() {
    print_separator("Example 3: Layer Priority");

    KeyView kv{.group = "priority", .key = "test"};

    // Set in System layer (lowest priority)
    (void)ConfigStore::instance().set(kv, std::string("system_value"), Layer::System);
    info(std::format("After System set: {}", ConfigStore::instance().query<std::string>(kv, "")));

    // Set in User layer (overrides System)
    (void)ConfigStore::instance().set(kv, std::string("user_value"), Layer::User);
    info(std::format("After User set: {}", ConfigStore::instance().query<std::string>(kv, "")));

    // Set in App layer (overrides User)
    (void)ConfigStore::instance().set(kv, std::string("app_value"), Layer::App);
    info(std::format("After App set: {}", ConfigStore::instance().query<std::string>(kv, "")));

    // Set in Temp layer (highest priority, in-memory only)
    (void)ConfigStore::instance().set(kv, std::string("temp_value"), Layer::Temp);
    info(std::format("After Temp set: {}", ConfigStore::instance().query<std::string>(kv, "")));

    // Clear Temp to fall back to App
    ConfigStore::instance().clear_layer(Layer::Temp);
    info(std::format("After Temp cleared: {}", ConfigStore::instance().query<std::string>(kv, "")));
}

// ============================================================================
// Example 4: Key Registration
// ============================================================================

void example_key_registration() {
    print_separator("Example 4: Key Registration");

    // Register a key with initial value
    Key max_size_key{.full_key = "file.max_size", .full_description = "Maximum file size in MB"};
    auto result = ConfigStore::instance().register_key(max_size_key, 100, Layer::User);

    if (result == RegisterResult::KeyRegisteredSuccess) {
        info("Key registered successfully");
        info("Key 'file.max_size' registered");
    }

    // Query the registered key
    int max_size =
        ConfigStore::instance().query<int>(KeyView{.group = "file", .key = "max_size"}, 50);
    info(std::format("Max file size: {} MB", max_size));

    // Try to register again (should fail)
    result = ConfigStore::instance().register_key(max_size_key, 200, Layer::User);
    if (result == RegisterResult::KeyAlreadyIn) {
        info("Key already exists (as expected)");
        info("Attempted to register existing key - correctly rejected");
    }
}

// ============================================================================
// Example 5: Watchers (Change Notification)
// ============================================================================

void example_watchers() {
    print_separator("Example 5: Watchers");

    // Add a watcher for all settings under "app"
    WatcherHandle handle = ConfigStore::instance().watch(
        "app.*",
        [](const Key& key, const std::any* old_val, const std::any* new_val, Layer layer) {
            std::string msg = std::format("Watcher triggered: {}", key.full_key);
            if (old_val && old_val->type() == typeid(std::string)) {
                msg += std::format(" (old: {})", std::any_cast<std::string>(*old_val));
            }
            if (new_val && new_val->type() == typeid(std::string)) {
                msg += std::format(" (new: {})", std::any_cast<std::string>(*new_val));
            }
            msg += std::format(" [layer: {}]", static_cast<int>(layer));
            info(msg);
        },
        NotifyPolicy::Immediate);

    // Trigger the watcher
    (void)ConfigStore::instance().set(KeyView{.group = "app", .key = "language"},
                                      std::string("en_US"));
    (void)ConfigStore::instance().set(KeyView{.group = "app", .key = "language"},
                                      std::string("zh_CN"));

    // Remove watcher
    ConfigStore::instance().unwatch(handle);
    info("Watcher removed");
}

// ============================================================================
// Example 6: Direct Layer Queries
// ============================================================================

void example_layer_specific_queries() {
    print_separator("Example 6: Layer-Specific Queries");

    KeyView kv{.group = "direct", .key = "value"};

    // Set in different layers
    (void)ConfigStore::instance().set(kv, 1, Layer::System);
    (void)ConfigStore::instance().set(kv, 2, Layer::User);
    (void)ConfigStore::instance().set(kv, 3, Layer::App);

    // Query specific layers
    auto system_val = ConfigStore::instance().query<int>(kv, Layer::System, 0);
    auto user_val = ConfigStore::instance().query<int>(kv, Layer::User, 0);
    auto app_val = ConfigStore::instance().query<int>(kv, Layer::App, 0);

    info(std::format("System layer: {}", system_val));
    info(std::format("User layer: {}", user_val));
    info(std::format("App layer: {}", app_val));

    // Priority query (returns App)
    auto priority_val = ConfigStore::instance().query<int>(kv, 0);
    info(std::format("Priority query (highest): {}", priority_val));
}

// ============================================================================
// Example 7: Manual Notification
// ============================================================================

void example_manual_notification() {
    print_separator("Example 7: Manual Notification");

    int callback_count = 0;

    // Add a watcher with manual notification policy
    WatcherHandle handle = ConfigStore::instance().watch(
        "manual.*",
        [&callback_count](const Key&, const std::any*, const std::any*, Layer) {
            callback_count++;
            info(std::format("Manual watcher called (count: {})", callback_count));
        },
        NotifyPolicy::Manual);

    // These won't trigger the watcher immediately
    bool ok1 = ConfigStore::instance().set(KeyView{.group = "manual", .key = "test1"}, 1,
                                           Layer::App, NotifyPolicy::Manual);
    bool ok2 = ConfigStore::instance().set(KeyView{.group = "manual", .key = "test2"}, 2,
                                           Layer::App, NotifyPolicy::Manual);
    info(std::format("Set results: test1={}, test2={}", ok1, ok2));

    info(std::format("Pending changes: {}", ConfigStore::instance().pending_changes()));

    // Manually trigger notification
    info("Calling notify()... ");
    auto result = ConfigStore::instance().notify();
    if (result == NotifyResult::NotifySuccess) {
        info("success!");
    } else {
        info(std::format("failed (code: {})", static_cast<int>(result)));
    }

    ConfigStore::instance().unwatch(handle);
}

// ============================================================================
// Main
// ============================================================================

int main() {
    // Initialize logger first
    run_logger_init();

    info("============================================");
    info("  CFConfig Module Example");
    info("============================================");
    info("Starting CFConfig example");

    try {
        example_basic_operations();
        example_types();
        example_layer_priority();
        example_key_registration();
        example_watchers();
        example_layer_specific_queries();
        example_manual_notification();

        // Sync to disk
        ConfigStore::instance().sync();
        info("Config synced to disk");

        print_separator("Example Complete");
        info("All examples completed successfully!");

    } catch (const std::exception& e) {
        error(std::format("Error: {}", e.what()));
        error(std::format("Exception: {}", e.what()));
        return 1;
    }

    return 0;
}
