/**
 * @file    desktop/main/init/init.h
 * @brief   Provides early initialization functions for the desktop session.
 *
 * Handles early initialization stages including config boot (stage 0) and
 * logger boot (stage 1), plus resource release and handoff to formal stage.
 *
 * @author  <git:author-name or "N/A">
 * @date    <git:last-commit-date or "N/A">
 * @version <git:last-tag-or-commit or "N/A">
 * @since   N/A
 * @ingroup desktop_main
 */

#pragma once

namespace cf::desktop::init_session {

/**
 * @brief   Runs early initialization stages for the desktop session.
 *
 * Initializes core subsystems in staging: early config boot (stage 0) and
 * early logger boot (stage 1).
 *
 * @throws  None
 * @note    None
 * @warning None
 * @since   N/A
 * @ingroup desktop_main
 */
void RunEarlyInit();

/**
 * @brief   Finalizes the early initialization phase in embedded desktop context.
 *
 * Releases embedded desktop resources, passes objects to formal stage,
 * transfers configurations to formal config stores, and handles other
 * transition tasks.
 *
 * @throws  None
 * @note    None
 * @warning None
 * @since   N/A
 * @ingroup desktop_main
 */
void ReleaseEarlyInit();

} // namespace cf::desktop::init_session
