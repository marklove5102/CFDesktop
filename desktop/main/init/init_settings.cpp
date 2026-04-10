#include "init_settings.h"
#include "early_handle/early_handle.h"

namespace cf::desktop::init_session {

void InitInfoHandle::passedEarlyBootInfo(early_stage::EarlyHandle& early_handle) {
    auto earlies = early_handle.unlockEarlySettings();
    early_settings_ = earlies->unlock_early_settings();
    /* Releases the Early settings... */
}

void InitInfoHandle::setBootWidget(QWidget* widget) {
    boot_widget_ = widget;
}

QWidget* InitInfoHandle::unlockBootWidget() {
    QWidget* p = boot_widget_;
    boot_widget_ = nullptr;
    return p;
}

QString InitInfoHandle::root_position() const {
    return early_settings_->value("desktop/root").value<QString>();
}

} // namespace cf::desktop::init_session