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

/* Staged inits */

/**
 * @brief   Runs the staged initialization sequence for the desktop session.
 *
 * Executes the full initialization pipeline across multiple stages, including
 * resource loading, subsystem initialization, and service startup.
 *
 * @throws  None
 * @note    This function should be called after early initialization completes.
 * @warning Must not be called more than once during a session.
 * @since   N/A
 * @ingroup desktop_main
 */
void RunStageInit();

/**
 * @brief   Releases resources allocated during staged initialization.
 *
 * Unlocks and deletes the boot widget to free memory after the staged
 * initialization phase completes.
 *
 * @throws  None
 * @note    The boot widget pointer retrieved from InitInfoHandle is deleted
 *          after being unlocked.
 * @warning Must only be called after the staged initialization completes.
 * @since   N/A
 * @ingroup desktop_main
 */
void ReleaseStageInitOldResources();
} // namespace cf::desktop::init_session
