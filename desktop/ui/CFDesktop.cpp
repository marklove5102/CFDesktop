/**
 * @file CFDesktop.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Welcomes Center!
 * @version 0.1
 * @date 2026-03-25
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "CFDesktop.h"
#include "CFDesktopEntity.h"
#include "cflog.h"

namespace cf::desktop {

CFDesktop::CFDesktop(CFDesktopEntity* entity) : weak_ptr_factory_(this) {
    log::traceftag("CFDesktop Self", "Creating CFDesktop...");
};

void CFDesktop::register_desktop_resources(InitResources& resources) {
    log::traceftag("CFDesktop Self", "Register Desktop Resources from remote");
    panel_manager_ = resources.panel_manager_;
    shell_layer_ = resources.shell_layer_;
}

CFDesktop::~CFDesktop() {
    log::traceftag("CFDesktop Self", "Destroying Self");
};

bool CFDesktop::component_available(const DesktopComponent d) const noexcept {
    switch (d) {
        case DesktopComponent::Common:
            return !panel_manager_ && !shell_layer_;
        case DesktopComponent::PanelManager:
            return !panel_manager_;
        case DesktopComponent::ShellLayer:
            return !shell_layer_;
    }
    return false;
}

} // namespace cf::desktop
