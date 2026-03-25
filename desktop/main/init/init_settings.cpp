#include "init_settings.h"
#include "early_handle/early_handle.h"

namespace cf::desktop::init_session {

void InitInfoHandle::passedEarlyBootInfo(early_stage::EarlyHandle& early_handle) {
    auto earlies = early_handle.unlockEarlySettings();
    early_settings_ = earlies->unlock_early_settings();
    /* Releases the Early settings... */
}
} // namespace cf::desktop::init_session