/**
 * @file config_impl.h
 * @brief Internal implementation of ConfigStore (Pimpl).
 *
 * Provides the ConfigStoreImpl class that manages configuration storage,
 * caching, layer management, and watcher notifications.
 *
 * @date 2026-03-17
 * @version 1.0
 */

#pragma once

#include "cfconfig/cfconfig_path_provider.h"
#include "cfconfig/cfconfig_result.h"
#include "cfconfig/cfconfig_watcher.h"
#include "cfconfig_key.h"
#include "cfconfig_layer.h"
#include "cfconfig_notify_policy.h"
#include "impl/config_backend.h"
#include <QString>
#include <any>
#include <atomic>
#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace cf::config {

/**
 * @brief Watcher entry for pattern matching.
 */
struct WatcherEntry {
    std::string pattern;  ///< Pattern to match (supports * wildcard)
    Watcher callback;     ///< Callback function
    NotifyPolicy policy;  ///< When to trigger this watcher
    WatcherHandle handle; ///< Unique handle for this watcher
};

/**
 * @brief Pending change for manual notification.
 */
struct PendingChange {
    Key key;
    std::any old_value;
    std::any new_value;
    Layer from_layer;
};

/**
 * @brief Deferred watcher event for callback execution after lock release.
 */
struct DeferredWatcherEvent {
    Watcher callback;
    Key key;
    std::any old_value;
    std::any new_value;
    Layer from_layer;
    bool has_old_value;
    bool has_new_value;
};

/**
 * @brief Internal implementation of ConfigStore.
 *
 * Manages four-layer configuration storage, caching, watchers,
 * and persistence operations.
 *
 * @note Thread-safe for all operations.
 * @note Not part of the public API.
 */
class ConfigStoreImpl {
  public:
    /**
     * @brief Constructs a new ConfigStoreImpl with default path provider.
     *
     * Initializes backends for each layer using DesktopConfigStorePathProvider:
     * - System: /etc/cfdesktop/system.ini (read-write, may not exist)
     * - User: ~/.config/cfdesktop/user.ini (read-write, created if needed)
     * - App: config/app.ini (read-write, relative path)
     * - Temp: Memory only
     */
    ConfigStoreImpl();

    /**
     * @brief Constructs a new ConfigStoreImpl with custom path provider.
     *
     * @param[in] path_provider Custom path provider for config file locations.
     *                           Allows tests and other projects to customize paths.
     */
    explicit ConfigStoreImpl(std::shared_ptr<IConfigStorePathProvider> path_provider);

    /**
     * @brief Destroys the ConfigStoreImpl.
     *
     * Syncs all dirty layers before destruction.
     */
    ~ConfigStoreImpl();

    // Delete copy and move
    ConfigStoreImpl(const ConfigStoreImpl&) = delete;
    ConfigStoreImpl& operator=(const ConfigStoreImpl&) = delete;
    ConfigStoreImpl(ConfigStoreImpl&&) = delete;
    ConfigStoreImpl& operator=(ConfigStoreImpl&&) = delete;

    /* ========== Query operations ========== */

    /**
     * @brief Query a configuration value with a fallback default.
     *
     * Searches all layers (merged view) for the given key and returns
     * the first match found. If the key does not exist in any layer,
     * @p default_value is returned instead.
     *
     * @param[in] key           The configuration key to look up.
     * @param[in] default_value The value to return if the key is not found.
     *
     * @return The queried value, or @p default_value if the key is absent.
     */
    std::any query(const std::string& key, const std::any& default_value);

    /**
     * @brief Query a configuration value from a specific layer.
     *
     * @param[in] key   The configuration key to look up.
     * @param[in] layer The layer to search.
     *
     * @return The value stored in the specified layer.
     */
    std::any query(const std::string& key, Layer layer);

    /**
     * @brief Check whether a key exists in any layer.
     *
     * @param[in] key The configuration key to check.
     *
     * @return True if the key exists in at least one layer, false otherwise.
     */
    bool has_key(const std::string& key);

    /**
     * @brief Check whether a key exists in a specific layer.
     *
     * @param[in] key   The configuration key to check.
     * @param[in] layer The layer to search.
     *
     * @return True if the key exists in the specified layer, false otherwise.
     */
    bool has_key(const std::string& key, Layer layer);

    /* ========== Write operations ========== */

    /**
     * @brief Set a key-value pair in the specified layer.
     *
     * @param[in] key            The configuration key to set.
     * @param[in] value          The value to store.
     * @param[in] layer          The target layer.
     * @param[in] notify_policy  Controls when watchers are notified.
     *
     * @return True if the value was set successfully, false otherwise.
     */
    bool set(const std::string& key, const std::any& value, Layer layer,
             NotifyPolicy notify_policy);

    /**
     * @brief Register a new key with an initial value in a layer.
     *
     * @param[in] key            The key descriptor to register.
     * @param[in] init_value     The initial value for the key.
     * @param[in] layer          The target layer.
     * @param[in] notify_policy  Controls when watchers are notified.
     *
     * @return The result of the registration attempt.
     */
    RegisterResult register_key(const Key& key, const std::any& init_value, Layer layer,
                                NotifyPolicy notify_policy);

    /**
     * @brief Unregister a key from a layer.
     *
     * @param[in] key            The key descriptor to unregister.
     * @param[in] layer          The layer to remove the key from.
     * @param[in] notify_policy  Controls when watchers are notified.
     *
     * @return The result of the unregistration attempt.
     */
    UnRegisterResult unregister_key(const Key& key, Layer layer, NotifyPolicy notify_policy);

    /**
     * @brief Clear all layers and cache.
     */
    void clear();

    /**
     * @brief Clear a specific layer.
     *
     * @param[in] layer The layer to clear.
     */
    void clear_layer(Layer layer);

    /* ========== Watcher operations ========== */

    /**
     * @brief Register a watcher callback for keys matching a pattern.
     *
     * @param[in] pattern  The key pattern to watch (supports * wildcard).
     * @param[in] callback The function to call when a matching key changes.
     * @param[in] policy   When to trigger this watcher.
     *
     * @return A handle that can be used to remove the watcher later.
     */
    WatcherHandle watch(const std::string& pattern, Watcher callback, NotifyPolicy policy);

    /**
     * @brief Remove a previously registered watcher.
     *
     * @param[in] handle The watcher handle returned by watch().
     */
    void unwatch(WatcherHandle handle);

    /**
     * @brief Manually trigger all pending watcher notifications.
     *
     * @return The result of the notification dispatch.
     */
    NotifyResult notify();

    /**
     * @brief Get the number of pending changes awaiting notification.
     *
     * @return The count of pending changes.
     */
    std::size_t pending_changes() const;

    /* ========== Persistence operations ========== */

    /**
     * @brief Persist all dirty layers to disk.
     *
     * @param[in] async If true, perform the write asynchronously; otherwise
     *                  block until complete.
     */
    void sync(bool async);

    /**
     * @brief Re-read all layers from disk, discarding in-memory changes.
     */
    void reload();

  private:
    /**
     * @brief Match a key against a pattern.
     *
     * Supports * wildcard matching.
     */
    static bool match_pattern(const std::string& pattern, const std::string& key);

    /**
     * @brief Get backend for a layer.
     *
     * Returns nullptr for Temp layer (memory only).
     */
    IConfigBackend* get_backend(Layer layer);

    /**
     * @brief Mark a layer as dirty (needs syncing).
     */
    void mark_dirty(Layer layer);

    /**
     * @brief Convert std::any to QVariant for backend storage.
     */
    static QVariant anyToQVariant(const std::any& value);

    /* ========== Internal lock-free implementations ========== */
    bool set_impl(const std::string& key, const std::any& value, Layer layer,
                  NotifyPolicy notify_policy);
    RegisterResult register_key_impl(const Key& key, const std::any& init_value, Layer layer,
                                     NotifyPolicy notify_policy);
    UnRegisterResult unregister_key_impl(const Key& key, Layer layer, NotifyPolicy notify_policy);
    void clear_layer_impl(Layer layer);
    void clear_impl();

    /**
     * @brief Trigger watchers for a key change (called with lock held).
     */
    void trigger_watchers(const Key& key, const std::any* old_value, const std::any* new_value,
                          Layer layer);

    /**
     * @brief Execute deferred watcher callbacks (called without lock).
     */
    void execute_deferred_watchers();

  private:
    // Thread safety
    mutable std::shared_mutex mutex_;
    std::mutex deferred_mutex_; ///< Mutex for deferred watcher events

    // Path provider for config file locations
    std::shared_ptr<IConfigStorePathProvider> path_provider_;

    // Cache for all layers (Temp is cache-only)
    std::unordered_map<std::string, std::any> cache_;

    // Layer storage (format-agnostic backends)
    std::unique_ptr<IConfigBackend> settings_system_;
    std::unique_ptr<IConfigBackend> settings_user_;
    std::unique_ptr<IConfigBackend> settings_app_;

    // Dirty flags for each layer
    std::array<bool, 4> dirty_flags_{false, false, false, false};

    // Watchers
    std::vector<WatcherEntry> watchers_;
    std::atomic<WatcherHandle> next_handle_{1};

    // Pending changes for Manual notification
    std::vector<PendingChange> pending_changes_;

    // Deferred watcher events (executed after lock release)
    std::vector<DeferredWatcherEvent> deferred_events_;
};

} // namespace cf::config
