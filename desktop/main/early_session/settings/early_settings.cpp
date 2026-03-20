#include "settings/early_settings.h"
#include "file_op.h"
#include "log/log_helper.h"

namespace cf::desktop::early_settings {

EarlySettings::EarlySettings(const QString& early_config_path)
    : early_settings(early_config_path, QSettings::IniFormat) {}

QString EarlySettings::get_boot_logger_path() const {
    if (!valid()) {
        return {};
    }

    if (!this_session_logfile_path.isEmpty()) {
        return this_session_logfile_path;
    }

    const auto log_file_rev = base::filesystem::concat_filepath(
        early_settings.value("logger/dirent", "").value<QString>(), logger_helper::log_filename());
    const auto log_file =
        base::filesystem::concat_filepath(base::filesystem::app_runtime_dir(), log_file_rev);

    this_session_logfile_path = log_file;
    return log_file;
}

} // namespace cf::desktop::early_settings
