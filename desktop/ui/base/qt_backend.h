/**
 * @file    desktop/ui/base/qt_backend.h
 * @brief   Platform-specific Qt backend enumeration and detection.
 *
 * Declares enum types for Qt backend variants on Linux (X11/Wayland) and
 * Windows, along with a runtime backend detection function.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup none
 */

#pragma once
#include "base/macro/system_judge.h"

namespace cf::desktop::backend {
#ifdef CFDESKTOP_OS_LINUX

/**
 * @brief  Qt backend types available on Linux platforms.
 *
 * Represents the underlying windowing system used by the Qt framework.
 * The backend affects rendering, input handling, and platform integration.
 *
 * @ingroup none
 */
enum class LinuxQtBackend {
    X11,     ///< X11 windowing system backend.
    Wayland, ///< Wayland compositor backend.
    Unknown  ///< Backend could not be determined or is unsupported.
};

/**
 * @brief  Detects the current Qt backend at runtime.
 *
 * Queries the Qt platform integration to determine whether the application
 * is running under X11 or Wayland.
 *
 * @return          Detected Qt backend for the current session.
 * @throws          None
 * @note            Result may vary depending on environment variables and
 *                  session configuration.
 * @warning         None
 * @since           N/A
 * @ingroup         none
 */
LinuxQtBackend GetBackend();

#elifdef CFDESKTOP_OS_WINDOWS

/**
 * @brief  Qt backend type for Windows platforms.
 *
 * Represents the standard desktop backend used by Qt on Windows.
 *
 * @ingroup none
 */
enum class WindowsQtBackend {
    Typical ///< Standard Windows desktop backend.
};

/**
 * @brief  Returns the Qt backend for Windows platforms.
 *
 * Provides a consistent interface with Linux, returning the standard
 * Windows backend.
 *
 * @return          Always returns WindowsQtBackend::Typical.
 * @throws          None
 * @note            Windows uses a single consistent backend type.
 * @warning         None
 * @since           N/A
 * @ingroup         none
 */
inline WindowsQtBackend GetBackend() {
    return WindowsQtBackend::Typical;
}

#endif

} // namespace cf::desktop::backend
