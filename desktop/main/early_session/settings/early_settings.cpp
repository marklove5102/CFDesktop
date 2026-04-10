#include "settings/early_settings.h"
#include "file_op.h"
#include "log/log_helper.h"
#include <memory>

namespace cf::desktop::early_settings {

EarlySettings::EarlySettings(const QString& early_config_path)
    : early_settings(std::make_unique<QSettings>(early_config_path, QSettings::IniFormat)) {}

bool EarlySettings::valid() const {
    if (!early_settings)
        return false;
    return early_settings->status() == QSettings::NoError;
}

QString EarlySettings::loadFrom() const {
    if (!early_settings)
        return "invalid settings";
    return early_settings->fileName();
}

QString EarlySettings::get_boot_logger_path() const {
    if (!valid()) {
        return {};
    }

    if (!this_session_logfile_path.isEmpty()) {
        return this_session_logfile_path;
    }

    const auto log_file_rev = base::filesystem::concat_filepath(
        early_settings->value("logger/dirent", "").value<QString>(), logger_helper::log_filename());
    const auto log_file =
        base::filesystem::concat_filepath(base::filesystem::app_runtime_dir(), log_file_rev);

    this_session_logfile_path = log_file;
    return log_file;
}

QString EarlySettings::get_desktop_root() const {
    if (!valid()) {
        return {};
    }

    if (!desktop_root.isEmpty()) {
        return desktop_root;
    }

    desktop_root = early_settings->value("desktop/root", "").value<QString>();
    return desktop_root;
}

std::unique_ptr<QSettings> EarlySettings::unlock_early_settings() {
    return std::move(early_settings);
}

} // namespace cf::desktop::early_settings
