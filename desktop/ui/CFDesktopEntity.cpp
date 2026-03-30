#include "CFDesktopEntity.h"
#include "CFDesktop.h"
#include "CFDesktopWindowProxy.h"
#include "IDesktopDisplaySizeStrategy.h"
#include "IDesktopPropertyStrategy.h"
#include "base/weak_ptr/weak_ptr.h"
#include "cflog.h"
#include "components/DisplayServerBackendFactory.h"
#include "components/IDisplayServerBackend.h"
#include "platform/DesktopPropertyStrategyFactory.h"
#include "platform/display_backend_helper.h"
#include <memory>

namespace cf::desktop {

std::unique_ptr<CFDesktopEntity> CFDesktopEntity::global_instance_;

CFDesktopEntity& CFDesktopEntity::instance() {
    if (!global_instance_) {
        global_instance_.reset(new CFDesktopEntity);
    }
    return *global_instance_;
}

CFDesktopEntity::CFDesktopEntity()
    : QObject(nullptr), desktop_entity_(new CFDesktop(this)),
      platform_factory_(std::make_unique<platform_strategy::PlatformFactory>()) {
    // Register the platform display backend creator into the factory
    auto api = platform::native_display();
    if (api.creator_func) {
        DisplayServerBackendFactory::instance().register_creator(std::move(api.creator_func),
                                                                 std::move(api.release_func));
    }
    log::tracef("Desktop Entity is created");
}

CFDesktopEntity::~CFDesktopEntity() {
    log::tracef("Dekstop Entuty is released");
}

void CFDesktopEntity::release() {
    global_instance_.reset();
}

CFDesktopEntity::RunsSetupResult CFDesktopEntity::run_init(RunsSetupMethod m) {
    // setup the window show proxy
    using StrategyType = platform_strategy::IDesktopPropertyStrategy::StrategyType;
    CFDesktopWindowProxy proxy;
    log::trace("Start Entity Init");

    auto display_policy_ =
        platform_factory_->create_unique<platform_strategy::IDesktopDisplaySizeStrategy>(
            StrategyType::DisplaySizePolicy);

    proxy.set_window_display_strategy(std::move(display_policy_));
    proxy.activate_window_display_strategy();

    // ── Windows: start display server backend for third-party window tracking ──
    display_backend_ = DisplayServerBackendFactory::instance().make_unique();

    if (display_backend_) {
        if (display_backend_->initialize(0, nullptr)) {
            auto wb = display_backend_->windowBackend();
            if (wb) {
                auto* raw = wb.Get();
                QObject::connect(raw, &IWindowBackend::window_came, this, [](WeakPtr<IWindow> win) {
                    if (win) {
                        cf::log::traceftag("CFDesktopEntity", "External window detected: %s",
                                           win->title().toStdString().c_str());
                    }
                });
                QObject::connect(raw, &IWindowBackend::window_gone, this,
                                 [](WeakPtr<IWindow> /*win*/) {
                                     cf::log::traceftag("CFDesktopEntity", "External window gone");
                                 });
            }
        } else {
            log::errorftag("CFDesktopEntity", "Display server backend init failed");
        }
    }

    log::trace("Entity Init");
    return RunsSetupResult::OK;
}

WeakPtr<CFDesktop> CFDesktopEntity::desktop_widget() const {
    return desktop_entity_->GetWeak();
}

} // namespace cf::desktop