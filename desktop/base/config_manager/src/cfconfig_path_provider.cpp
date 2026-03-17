/**
 * @file cfconfig_path_provider.cpp
 * @brief Implementation of ConfigStorePathProvider.
 *
 * @date 2026-03-17
 * @version 1.0
 */

#include "cfconfig/cfconfig_path_provider.h"
#include <QDir>

namespace cf::config {

// ============================================================================
// DesktopConfigStorePathProvider
// ============================================================================

DesktopConfigStorePathProvider::DesktopConfigStorePathProvider()
    : system_path_("/etc/cfdesktop/system.ini"), user_dir_(QDir::homePath() + "/.config/cfdesktop"),
      app_dir_("config") {}

DesktopConfigStorePathProvider::DesktopConfigStorePathProvider(const QString& app_base)
    : system_path_("/etc/cfdesktop/system.ini"), user_dir_(QDir::homePath() + "/.config/cfdesktop"),
      app_dir_(app_base) {}

DesktopConfigStorePathProvider::DesktopConfigStorePathProvider(const QString& organization,
                                                               const QString& application)
    : system_path_("/etc/" + organization + "/system.ini"),
      user_dir_(QDir::homePath() + "/.config/" + organization), app_dir_("config") {
    user_filename_ = application + ".ini";
    app_filename_ = application + ".ini";
}

QString DesktopConfigStorePathProvider::system_path() const {
    return system_path_;
}

QString DesktopConfigStorePathProvider::user_dir() const {
    return user_dir_;
}

QString DesktopConfigStorePathProvider::user_filename() const {
    return user_filename_;
}

QString DesktopConfigStorePathProvider::app_dir() const {
    return app_dir_;
}

QString DesktopConfigStorePathProvider::app_filename() const {
    return app_filename_;
}

bool DesktopConfigStorePathProvider::is_layer_enabled(int layer_index) const {
    // All layers enabled by default for Desktop
    (void)layer_index;
    return true;
}

} // namespace cf::config
