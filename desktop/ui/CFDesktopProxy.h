/**
 * @file    CFDesktopProxy.h
 * @brief   CFDesktop Proxy header for controlled desktop access.
 *
 * @details Provides the common interfaces for querying desktops.
 *          Proxy provides a common interface for visiting the components.
 *
 * @author  CFDesktop Team
 * @date    2026-03-25
 * @version 0.1
 * @since   0.1
 * @ingroup desktop_ui
 */
#pragma once
#include "base/weak_ptr/weak_ptr.h"

namespace cf::desktop {
class CFDesktop;

/**
 * @brief   Provides the common interfaces for querying desktops.
 *
 * @details Proxy provides a common interface for visiting the components.
 *
 * @throws        None
 * @note          None
 * @warning       None
 * @since         0.1
 * @ingroup       desktop_ui
 */
class CFDesktopProxy {
  public:
    /* Make A Proxy Must handle a desktop weak ref */
    CFDesktopProxy(WeakPtr<CFDesktop> desktop);
    ~CFDesktopProxy();

  private:
    WeakPtr<CFDesktop> desktop_;
};

} // namespace cf::desktop
