/**
 * @file    linux_wsl_platform.h
 * @brief   Platform-specific factory API for Linux/WSL desktop environments.
 *
 * Provides the native implementation entry point for creating desktop property
 * strategies on Linux/WSL platforms.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup none
 */

#pragma once
#include "platform_helper.h"

namespace cf::desktop::platform_strategy {

/**
 * @brief  Returns the native platform factory API implementation.
 *
 * Provides access to the platform-specific factory for creating desktop
 * property strategies on Linux/WSL systems.
 *
 * @return  The platform factory API structure.
 * @throws  None
 * @note    None
 * @warning None
 * @since   N/A
 * @ingroup none
 */
PlatformFactoryAPI native_impl();

} // namespace cf::desktop::platform_strategy
