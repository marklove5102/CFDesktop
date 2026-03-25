/**
 * @file CFDesktop.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Welcomes Center!
 * @version 0.1
 * @date 2026-03-25
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include "CFDesktopProxy.h"
#include "base/weak_ptr/weak_ptr_factory.h"

#include <QWidget>
#include <memory>

namespace cf::desktop {
class PanelManager;
class ShellLayer;
class CFDesktopIniter;

/**
 * @brief Main desktop class representing the CFDesktop application.
 *
 * @details Manages desktop components including panel manager and shell layer.
 *          Provides controlled access through CFDesktopProxy for safety.
 *
 * @throws        None
 * @note          External access must go through GetProxy()
 * @warning       Direct access is restricted; use CFDesktopProxy
 * @since         0.1
 * @ingroup       desktop_ui
 */
class CFDesktop : public QWidget {
    Q_OBJECT
  public:
    friend class CFDesktopProxy; // Proxy Friendly
    // init with initers
    // like, register panel manager
    // shell layers... and so on!
    friend class CFDesktopIniter;

    /**
     * @brief Gets the proxy for controlled external access to the desktop.
     *
     * @return        Unique pointer to the desktop proxy.
     *
     * @throws        None
     * @note          External classes must use the proxy to access desktop
     * @warning       None
     * @since         0.1
     * @ingroup       desktop_ui
     */
    std::unique_ptr<CFDesktopProxy> GetProxy();

    /**
     * @brief Desktop component identifiers for availability checking.
     *
     * @details Component types:
     *          - Common: Minimal core components
     *          - PanelManager: Panel management system
     *          - ShellLayer: Shell layer interface
     *
     * @since         0.1
     * @ingroup       desktop_ui
     */
    enum class DesktopComponent { Common, PanelManager, ShellLayer };

    /**
     * @brief Checks if a specific desktop component is available.
     *
     * @param[in]     d The desktop component to check (default: Common).
     *
     * @return        True if the component is available, false otherwise.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       desktop_ui
     */
    bool component_available(const DesktopComponent d = DesktopComponent::Common) const noexcept;

  private:
    /* Managed by Resources */
    struct InitResources {
        PanelManager* panel_manager_{nullptr};
        ShellLayer* shell_layer_{nullptr};
    };

    PanelManager* panel_manager_{nullptr};
    ShellLayer* shell_layer_{nullptr};

  private:
    CFDesktop(/* nullptr, actually always stand alone */);
    ~CFDesktop();

    // register the desktop sources
    void register_desktop_resources(InitResources& resources);

  private:
    cf::WeakPtrFactory<CFDesktop> weak_ptr_factory_;
};
} // namespace cf::desktop
