#include "CFDesktop.h"
namespace cf::desktop {
CFDesktopProxy::CFDesktopProxy(WeakPtr<CFDesktop> desktop) : desktop_(desktop) {}

CFDesktopProxy::~CFDesktopProxy() {}

} // namespace cf::desktop
