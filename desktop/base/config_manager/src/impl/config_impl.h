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
#include <QString>
#include <any>
#include <atomic>
#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

// Forward declarations for QSettings
class QSettings;

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
     * Initializes QSettings for each layer using DesktopConfigStorePathProvider:
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
     * @return None
     * @throws None
     * @note None
     * @warning None
     * @since 1.0
     * @ingroup config
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
     * @brief Query a configuration value by priority.
     *
     * Searches layers in order: Temp → App → User → System
     *
     * @param[in] key The full key to query
     * @param[in] default_value Value to return if key not found
     * @return The found value or default_value
     * @throws None
     * @note None
     * @warning None
     * @since 1.0
     * @ingroup config
     */
    std::any query(const std::string& key, const std::any& default_value);

    /**
     * @brief Query a configuration value from a specific layer.
     *
     * @param[in] key The full key to query
     * @param[in] layer The specific layer to query
     * @return std::any with the value, or std::any() if not found
     * @throws None
     * @note None
     * @warning None
     * @since 1.0
     * @ingroup config
     */
    std::any query(const std::string& key, Layer layer);

    /**
     * @brief Check if a key exists (searches all layers by priority).
     *
     * @param[in] key The full key to check
     * @return true if the key exists in any layer, false otherwise
     * @throws None
     * @note None
     * @warning None
     * @since 1.0
     * @ingroup config
     */
    bool has_key(const std::string& key);

    /**
     * @brief Check if a key exists in a specific layer.
     *
     * @param[in] key The full key to check
     * @param[in] layer The specific layer to check
     * @return true if the key exists in the specified layer, false otherwise
     * @throws None
     * @note None
     * @warning None
     * @since 1.0
     * @ingroup config
     */
    bool has_key(const std::string& key, Layer layer);

    /* ========== Write operations ========== */

    /**
     * @brief Set a configuration value.
     *
     * @param[in] key The full key to set
     * @param[in] value The value to set
     * @param[in] layer The target layer
     * @param[in] notify_policy Whether to notify immediately
     * @return true if successful, false otherwise
     * @throws None
     * @note None
     * @warning None
     * @since 1.0
     * @ingroup config
     */
    bool set(const std::string& key, const std::any& value, Layer layer,
             NotifyPolicy notify_policy);

    /**
     * @brief Register a key with an initial value.
     *
     * @param[in] key The key to register
     * @param[in] init_value The initial value for the key
     * @param[in] layer The target layer
     * @param[in] notify_policy Whether to notify immediately
     * @return RegisterResult indicating success or failure
     * @throws None
     * @note None
     * @warning None
     * @since 1.0
     * @ingroup config
     */
    RegisterResult register_key(const Key& key, const std::any& init_value, Layer layer,
                                NotifyPolicy notify_policy);

    /**
     * @brief Unregister (remove) a key.
     *
     * @param[in] key The key to unregister
     * @param[in] layer The target layer
     * @param[in] notify_policy Whether to notify immediately
     * @return UnRegisterResult indicating success or failure
     * @throws None
     * @note None
     * @warning None
     * @since 1.0
     * @ingroup config
     */
    UnRegisterResult unregister_key(const Key& key, Layer layer, NotifyPolicy notify_policy);

    /**
     * @brief Clear all layers (including Temp).
     */
    void clear();

    /**
     * @brief Clear a specific layer.
     */
    void clear_layer(Layer layer);

    /* ========== Watcher operations ========== */

    /**
     * @brief Add a watcher for a key pattern.
     *
     * @param[in] pattern Key pattern (supports * wildcard)
     * @param[in] callback Callback function
     * @param[in] policy When to trigger
     * @return Watcher handle for removal
     * @throws None
     * @note None
     * @warning None
     * @since 1.0
     * @ingroup config
     */
    WatcherHandle watch(const std::string& pattern, Watcher callback, NotifyPolicy policy);

    /**
     * @brief Remove a watcher.
     */
    void unwatch(WatcherHandle handle);

    /**
     * @brief Trigger all Manual watchers.
     *
     * @return NotifyResult indicating success/failure
     */
    NotifyResult notify();

    /**
     * @brief Get count of pending changes.
     *
     * @return The number of pending changes
     * @throws None
     * @note None
     * @warning None
     * @since 1.0
     * @ingroup config
     */
    std::size_t pending_changes() const;

    /* ========== Persistence operations ========== */

    /**
     * @brief Sync dirty layers to disk.
     *
     * @param[in] async If true, sync asynchronously (not implemented, always sync)
     * @return None
     * @throws None
     * @note None
     * @warning None
     * @since 1.0
     * @ingroup config
     */
    void sync(bool async);

    /**
     * @brief Reload configuration from disk.
     *
     * Clears Temp layer and cache, reloads from QSettings.
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
     * @brief Convert dot-separated key to QSettings path.
     *
     * "app.theme.name" → "app/theme/name"
     */
    static QString to_qsettings_path(const std::string& key);

    /**
     * @brief Get QSettings for a layer.
     *
     * Returns nullptr for Temp layer (memory only).
     */
    QSettings* get_settings(Layer layer);

    /**
     * @brief Mark a layer as dirty (needs syncing).
     */
    void mark_dirty(Layer layer);

    /* ========== Internal lock-free implementations ========== */
    /**
     * @brief Internal set implementation (caller must hold lock).
     *
     * This method does NOT acquire any locks. Caller must hold mutex_.
     */
    bool set_impl(const std::string& key, const std::any& value, Layer layer,
                  NotifyPolicy notify_policy);

    /**
     * @brief Internal register_key implementation (caller must hold lock).
     *
     * This method does NOT acquire any locks. Caller must hold mutex_.
     */
    RegisterResult register_key_impl(const Key& key, const std::any& init_value, Layer layer,
                                     NotifyPolicy notify_policy);

    /**
     * @brief Internal unregister_key implementation (caller must hold lock).
     *
     * This method does NOT acquire any locks. Caller must hold mutex_.
     */
    UnRegisterResult unregister_key_impl(const Key& key, Layer layer, NotifyPolicy notify_policy);

    /**
     * @brief Internal clear_layer implementation (caller must hold lock).
     *
     * This method does NOT acquire any locks. Caller must hold mutex_.
     */
    void clear_layer_impl(Layer layer);

    /**
     * @brief Internal clear implementation (caller must hold lock).
     *
     * This method does NOT acquire any locks. Caller must hold mutex_.
     */
    void clear_impl();

    /**
     * @brief Trigger watchers for a key change (called with lock held).
     *
     * Collects watcher events but defers callback execution until after lock release.
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

    // Layer storage
    std::unique_ptr<QSettings> settings_system_;
    std::unique_ptr<QSettings> settings_user_;
    std::unique_ptr<QSettings> settings_app_;

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
