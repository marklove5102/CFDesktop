#include "CFDesktop.h"
#include "CFDesktopEntity.h"

namespace cf::desktop {

CFDesktopProxy::CFDesktopProxy(WeakPtr<CFDesktop> desktop) : desktop_(desktop) {}

} // namespace cf::desktop
