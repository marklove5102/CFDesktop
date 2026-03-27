/**
 * @file    desktop/desktop_entry.h
 * @brief   Desktop entry point and exit result definitions.
 *
 * Provides the main entry point for booting the desktop session and defines
 * the possible exit result codes.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup none
 */

#pragma once

namespace cf::desktop {

/**
 * @brief  Exit result codes for the desktop session.
 * @ingroup none
 */
enum class DesktopExitResult {
    NORMAL_QUIT ///< Normal exit without errors.
};

/**
 * @brief  Boots the desktop session and initializes the desktop environment.
 *
 * Creates the desktop entity instance, initializes the session, displays the
 * desktop window, and releases early initialization resources.
 *
 * @return              Exit result code indicating the termination status.
 * @throws              None
 * @note                This function starts the main desktop event loop.
 * @warning             None
 * @since               N/A
 * @ingroup             none
 */
DesktopExitResult boot_desktop();

} // namespace cf::desktop
