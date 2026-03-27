#include "linux_wsl_platform.h"
#include "IDesktopPropertyStrategy.h"
#include "linux_wsl_factory.h"
#include "platform_helper.h"

namespace cf::desktop::platform_strategy {

PlatformFactoryAPI native_impl() {
    PlatformFactoryAPI api;
    api.creator_func = [](IDesktopPropertyStrategy::StrategyType t) {
        return wsl::WSLDeskProStrategyFactory::instance().create(t);
    };
    api.release_func = [](IDesktopPropertyStrategy* v) {
        wsl::WSLDeskProStrategyFactory::instance().release(v);
    };
    return api;
}

} // namespace cf::desktop::platform_strategy