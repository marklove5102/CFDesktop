/**
 * @file config_impl.cpp
 * @brief Internal implementation of ConfigStore (Pimpl).
 *
 * @date 2026-03-17
 * @version 1.0
 */

#include "impl/config_impl.h"
#include "cfconfig/cfconfig_path_provider.h"
#include "cfconfig/cfconfig_result.h"
#include "cfconfig_key.h"
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QString>
#include <array>
#include <memory>
#include <mutex>
#include <regex>

namespace cf::config {

namespace detail {

/**
 * @brief Convert dot-separated key to QSettings path.
 * "app.theme.name" → "app/theme/name"
 */
QString to_qsettings_path(const std::string& key) {
    QString qkey = QString::fromStdString(key);
    return qkey.replace('.', '/');
}

/**
 * @brief Match a key pattern against a key.
 * Supports * wildcard.
 */
bool match_pattern(const std::string& pattern, const std::string& key) {
    // Convert glob pattern to regex
    std::string regex_pattern = pattern;
    // Escape special regex characters except *
    size_t pos = 0;
    while ((pos = regex_pattern.find_first_of("^.${}+|()[]\\", pos)) != std::string::npos) {
        regex_pattern.insert(pos, "\\");
        pos += 2;
    }
    // Replace * with .*
    pos = 0;
    while ((pos = regex_pattern.find('*', pos)) != std::string::npos) {
        regex_pattern.replace(pos, 1, ".*");
        pos += 2;
    }
    // Anchor to start and end
    regex_pattern = "^" + regex_pattern + "$";

    try {
        std::regex re(regex_pattern);
        return std::regex_match(key, re);
    } catch (const std::regex_error&) {
        return false;
    }
}

} // namespace detail

// ============================================================================
// Constructor / Destructor
// ============================================================================

ConfigStoreImpl::ConfigStoreImpl()
    : ConfigStoreImpl(std::make_shared<DesktopConfigStorePathProvider>()) {}

ConfigStoreImpl::ConfigStoreImpl(std::shared_ptr<IConfigStorePathProvider> path_provider)
    : path_provider_(std::move(path_provider)) {
    // System layer: use path provider
    QString systemPath = path_provider_->system_path();
    if (!systemPath.isEmpty() && QFileInfo::exists(systemPath)) {
        settings_system_ = std::make_unique<QSettings>(systemPath, QSettings::IniFormat);
    }

    // User layer: use path provider
    QString userDir = path_provider_->user_dir();
    if (!userDir.isEmpty()) {
        QDir().mkpath(userDir);
        settings_user_ = std::make_unique<QSettings>(
            userDir + "/" + path_provider_->user_filename(), QSettings::IniFormat);
    }

    // App layer: use path provider
    QString appDir = path_provider_->app_dir();
    if (!appDir.isEmpty()) {
        QDir().mkpath(appDir);
        settings_app_ = std::make_unique<QSettings>(appDir + "/" + path_provider_->app_filename(),
                                                    QSettings::IniFormat);
    }
}

ConfigStoreImpl::~ConfigStoreImpl() {
    // Sync all dirty layers
    sync(false);
}

// ============================================================================
// Query operations
// ============================================================================

std::any ConfigStoreImpl::query(const std::string& key, const std::any& default_value) {
    std::shared_lock lock(mutex_);

    // Check cache first (Temp layer)
    auto it = cache_.find(key);
    if (it != cache_.end()) {
        return it->second;
    }

    // Query App layer
    if (settings_app_) {
        QString qkey = detail::to_qsettings_path(key);
        QVariant value = settings_app_->value(qkey);
        if (!value.isNull()) {
            cache_[key] = value;
            return value;
        }
    }

    // Query User layer
    if (settings_user_) {
        QString qkey = detail::to_qsettings_path(key);
        QVariant value = settings_user_->value(qkey);
        if (!value.isNull()) {
            cache_[key] = value;
            return value;
        }
    }

    // Query System layer
    if (settings_system_) {
        QString qkey = detail::to_qsettings_path(key);
        QVariant value = settings_system_->value(qkey);
        if (!value.isNull()) {
            cache_[key] = value;
            return value;
        }
    }

    return default_value;
}

std::any ConfigStoreImpl::query(const std::string& key, Layer layer) {
    std::shared_lock lock(mutex_);

    // Temp layer - check cache
    if (layer == Layer::Temp) {
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            return it->second;
        }
        return std::any();
    }

    QSettings* settings = get_settings(layer);
    if (!settings) {
        return std::any();
    }

    QString qkey = detail::to_qsettings_path(key);
    QVariant value = settings->value(qkey);

    if (value.isNull()) {
        return std::any();
    }

    return value;
}

bool ConfigStoreImpl::has_key(const std::string& key) {
    std::shared_lock lock(mutex_);

    // Check cache (Temp layer)
    if (cache_.find(key) != cache_.end()) {
        return true;
    }

    // Check App layer
    if (settings_app_) {
        QString qkey = detail::to_qsettings_path(key);
        if (settings_app_->contains(qkey)) {
            return true;
        }
    }

    // Check User layer
    if (settings_user_) {
        QString qkey = detail::to_qsettings_path(key);
        if (settings_user_->contains(qkey)) {
            return true;
        }
    }

    // Check System layer
    if (settings_system_) {
        QString qkey = detail::to_qsettings_path(key);
        if (settings_system_->contains(qkey)) {
            return true;
        }
    }

    return false;
}

bool ConfigStoreImpl::has_key(const std::string& key, Layer layer) {
    std::shared_lock lock(mutex_);

    if (layer == Layer::Temp) {
        return cache_.find(key) != cache_.end();
    }

    QSettings* settings = get_settings(layer);
    if (!settings) {
        return false;
    }

    QString qkey = detail::to_qsettings_path(key);
    return settings->contains(qkey);
}

// ============================================================================
// Write operations (public - acquire locks)
// ============================================================================

bool ConfigStoreImpl::set(const std::string& key, const std::any& value, Layer layer,
                          NotifyPolicy notify_policy) {
    std::unique_lock lock(mutex_);
    bool result = set_impl(key, value, layer, notify_policy);
    if (result) {
        lock.unlock();
        execute_deferred_watchers();
    }
    return result;
}

RegisterResult ConfigStoreImpl::register_key(const Key& key, const std::any& init_value,
                                             Layer layer, NotifyPolicy notify_policy) {
    std::unique_lock lock(mutex_);
    RegisterResult result = register_key_impl(key, init_value, layer, notify_policy);
    lock.unlock();
    execute_deferred_watchers();
    return result;
}

UnRegisterResult ConfigStoreImpl::unregister_key(const Key& key, Layer layer,
                                                 NotifyPolicy notify_policy) {
    std::unique_lock lock(mutex_);
    UnRegisterResult result = unregister_key_impl(key, layer, notify_policy);
    lock.unlock();
    execute_deferred_watchers();
    return result;
}

void ConfigStoreImpl::clear() {
    std::unique_lock lock(mutex_);
    clear_impl();
    lock.unlock();
    execute_deferred_watchers();
}

void ConfigStoreImpl::clear_layer(Layer layer) {
    std::unique_lock lock(mutex_);
    clear_layer_impl(layer);
    lock.unlock();
    execute_deferred_watchers();
}

// ============================================================================
// Write operations (internal lock-free implementations)
// ============================================================================

bool ConfigStoreImpl::set_impl(const std::string& key, const std::any& value, Layer layer,
                               NotifyPolicy notify_policy) {
    // Store old value for notification
    std::any old_value;
    bool had_old = false;
    auto it = cache_.find(key);
    if (it != cache_.end()) {
        old_value = it->second;
        had_old = true;
    }

    // Temp layer - just update cache
    if (layer == Layer::Temp) {
        cache_[key] = value;

        if (notify_policy == NotifyPolicy::Immediate) {
            Key k{.full_key = key, .full_description = key};
            trigger_watchers(k, had_old ? &old_value : nullptr, &value, layer);
        } else {
            pending_changes_.push_back({Key{.full_key = key, .full_description = key},
                                        had_old ? old_value : std::any(), value, layer});
        }
        return true;
    }

    // Other layers - update QSettings
    QSettings* settings = get_settings(layer);
    if (!settings) {
        return false;
    }

    QString qkey = detail::to_qsettings_path(key);

    // Get old value from QSettings if not in cache
    if (!had_old && settings->contains(qkey)) {
        old_value = settings->value(qkey);
        had_old = true;
    }

    // Update QSettings
    QVariant qvalue;
    if (value.type() == typeid(int)) {
        qvalue = QVariant::fromValue(std::any_cast<int>(value));
    } else if (value.type() == typeid(double)) {
        qvalue = QVariant::fromValue(std::any_cast<double>(value));
    } else if (value.type() == typeid(bool)) {
        qvalue = QVariant::fromValue(std::any_cast<bool>(value));
    } else if (value.type() == typeid(std::string)) {
        qvalue = QString::fromStdString(std::any_cast<std::string>(value));
    } else {
        // Try QVariant directly
        try {
            qvalue = std::any_cast<QVariant>(value);
        } catch (const std::bad_any_cast&) {
            return false;
        }
    }

    settings->setValue(qkey, qvalue);
    cache_[key] = value;
    mark_dirty(layer);

    // Notify watchers
    if (notify_policy == NotifyPolicy::Immediate) {
        Key k{.full_key = key, .full_description = key};
        trigger_watchers(k, had_old ? &old_value : nullptr, &value, layer);
    } else {
        pending_changes_.push_back({Key{.full_key = key, .full_description = key},
                                    had_old ? old_value : std::any(), value, layer});
    }

    return true;
}

RegisterResult ConfigStoreImpl::register_key_impl(const Key& key, const std::any& init_value,
                                                  Layer layer, NotifyPolicy notify_policy) {
    // Check if key already exists in cache
    if (cache_.find(key.full_key) != cache_.end()) {
        return RegisterResult::KeyAlreadyIn;
    }

    // Check QSettings for non-Temp layers
    if (layer != Layer::Temp) {
        QSettings* settings = get_settings(layer);
        if (settings) {
            QString qkey = detail::to_qsettings_path(key.full_key);
            if (settings->contains(qkey)) {
                return RegisterResult::KeyAlreadyIn;
            }
        }
    }

    // Register the key using internal set
    set_impl(key.full_key, init_value, layer, notify_policy);
    return RegisterResult::KeyRegisteredSuccess;
}

UnRegisterResult ConfigStoreImpl::unregister_key_impl(const Key& key, Layer layer,
                                                      NotifyPolicy notify_policy) {
    bool existed = false;
    std::any old_value;

    // Check and remove from cache
    auto it = cache_.find(key.full_key);
    if (it != cache_.end()) {
        old_value = it->second;
        cache_.erase(it);
        existed = true;
    }

    // Remove from QSettings for non-Temp layers
    if (layer != Layer::Temp) {
        QSettings* settings = get_settings(layer);
        if (settings) {
            QString qkey = detail::to_qsettings_path(key.full_key);
            if (settings->contains(qkey)) {
                if (!existed) {
                    old_value = settings->value(qkey);
                }
                settings->remove(qkey);
                existed = true;
                mark_dirty(layer);
            }
        }
    }

    if (!existed) {
        return UnRegisterResult::KeyUnexisted;
    }

    // Notify watchers
    if (notify_policy == NotifyPolicy::Immediate) {
        trigger_watchers(key, &old_value, nullptr, layer);
    } else {
        pending_changes_.push_back({key, old_value, std::any(), layer});
    }

    return UnRegisterResult::KeyUnRegisteredSuccess;
}

void ConfigStoreImpl::clear_impl() {
    cache_.clear();

    if (settings_system_) {
        settings_system_->clear();
    }
    if (settings_user_) {
        settings_user_->clear();
    }
    if (settings_app_) {
        settings_app_->clear();
    }

    dirty_flags_.fill(true);
    pending_changes_.clear();
}

void ConfigStoreImpl::clear_layer_impl(Layer layer) {
    if (layer == Layer::Temp) {
        cache_.clear();
        return;
    }

    QSettings* settings = get_settings(layer);
    if (settings) {
        settings->clear();
        mark_dirty(layer);
    }
}

// ============================================================================
// Watcher operations
// ============================================================================

WatcherHandle ConfigStoreImpl::watch(const std::string& pattern, Watcher callback,
                                     NotifyPolicy policy) {
    std::unique_lock lock(mutex_);
    WatcherHandle handle = next_handle_.fetch_add(1);
    watchers_.push_back({pattern, std::move(callback), policy, handle});
    return handle;
}

void ConfigStoreImpl::unwatch(WatcherHandle handle) {
    std::unique_lock lock(mutex_);
    auto it = std::remove_if(watchers_.begin(), watchers_.end(),
                             [handle](const WatcherEntry& e) { return e.handle == handle; });
    watchers_.erase(it, watchers_.end());
}

NotifyResult ConfigStoreImpl::notify() {
    std::unique_lock lock(mutex_);

    if (pending_changes_.empty()) {
        return NotifyResult::NothingWorthNotify;
    }

    // Collect all watcher events
    for (const auto& change : pending_changes_) {
        trigger_watchers(change.key,
                         change.old_value.type() == typeid(void) ? nullptr : &change.old_value,
                         change.new_value.type() == typeid(void) ? nullptr : &change.new_value,
                         change.from_layer);
    }

    pending_changes_.clear();
    lock.unlock();

    execute_deferred_watchers();
    return NotifyResult::NotifySuccess;
}

std::size_t ConfigStoreImpl::pending_changes() const {
    std::shared_lock lock(mutex_);
    return pending_changes_.size();
}

// ============================================================================
// Persistence operations
// ============================================================================

void ConfigStoreImpl::sync(bool async) {
    // Note: async parameter is reserved for future implementation
    // For now, all syncs are synchronous

    std::shared_lock lock(mutex_);

    if (settings_system_ && dirty_flags_[0]) {
        settings_system_->sync();
        dirty_flags_[0] = false;
    }
    if (settings_user_ && dirty_flags_[1]) {
        settings_user_->sync();
        dirty_flags_[1] = false;
    }
    if (settings_app_ && dirty_flags_[2]) {
        settings_app_->sync();
        dirty_flags_[2] = false;
    }
    // Temp layer (index 3) doesn't need syncing
}

void ConfigStoreImpl::reload() {
    std::unique_lock lock(mutex_);

    // Clear cache (this clears the Temp layer)
    cache_.clear();
    pending_changes_.clear();

    // Reload QSettings (they automatically reload on next access)
    // No explicit reload needed for QSettings
}

// ============================================================================
// Private helpers
// ============================================================================

QString ConfigStoreImpl::to_qsettings_path(const std::string& key) {
    return detail::to_qsettings_path(key);
}

bool ConfigStoreImpl::match_pattern(const std::string& pattern, const std::string& key) {
    return detail::match_pattern(pattern, key);
}

QSettings* ConfigStoreImpl::get_settings(Layer layer) {
    switch (layer) {
        case Layer::System:
            return settings_system_.get();
        case Layer::User:
            return settings_user_.get();
        case Layer::App:
            return settings_app_.get();
        case Layer::Temp:
            return nullptr; // Temp layer is memory only
    }
    return nullptr;
}

void ConfigStoreImpl::trigger_watchers(const Key& key, const std::any* old_value,
                                       const std::any* new_value, Layer layer) {
    // We're already under lock - collect events for deferred execution
    for (const auto& watcher : watchers_) {
        if (detail::match_pattern(watcher.pattern, key.full_key)) {
            DeferredWatcherEvent event;
            event.callback = watcher.callback;
            event.key = key;
            event.from_layer = layer;
            event.has_old_value = (old_value != nullptr);
            event.has_new_value = (new_value != nullptr);
            if (old_value)
                event.old_value = *old_value;
            if (new_value)
                event.new_value = *new_value;
            deferred_events_.push_back(std::move(event));
        }
    }
}

void ConfigStoreImpl::execute_deferred_watchers() {
    // Execute deferred callbacks without holding the main mutex
    std::vector<DeferredWatcherEvent> events;
    {
        std::lock_guard<std::mutex> lock(deferred_mutex_);
        events = std::move(deferred_events_);
        deferred_events_.clear();
    }

    // Now execute callbacks without any lock held
    for (const auto& event : events) {
        event.callback(event.key, event.has_old_value ? &event.old_value : nullptr,
                       event.has_new_value ? &event.new_value : nullptr, event.from_layer);
    }
}

void ConfigStoreImpl::mark_dirty(Layer layer) {
    int index = static_cast<int>(layer);
    if (index >= 0 && index < 4) {
        dirty_flags_[index] = true;
    }
}

} // namespace cf::config
