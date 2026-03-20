#pragma once

/**
 * @file    base/include/base/macro/build_type.h
 * @brief   Defines build type detection macros for the CFDesktop project.
 *
 * Provides macros that indicate the current build configuration (Debug,
 * Develop, or Release) based on preprocessor definitions.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup base_macro
 */

// CFDESKTOP_DEBUG_BUILD is defined when build type is Debug
#if defined(_CFDESKTOPDEBUG)
#    define CFDESKTOP_DEBUG_BUILD
#endif

#if defined(NDEBUG)
#    define CFDESKTOP_DEVELOP_BUILD
#endif