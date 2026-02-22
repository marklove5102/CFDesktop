/**
 * @file    base/include/base/macro/system_judge.h
 * @brief   Defines platform and architecture detection macros.
 *
 * Provides compile-time macros for detecting the operating system
 * and CPU architecture.
 *
 * @author  Charliechen114514
 * @date    2026-02-22
 * @version 0.1
 * @since   0.1
 * @ingroup base_macros
 */
#pragma once
#include <QtGlobal>

#ifdef Q_OS_WIN
#    define CFDESKTOP_OS_WINDOWS
#endif

// Linux platform detection
#ifdef Q_OS_LINUX
#    define CFDESKTOP_OS_LINUX
#endif

#ifdef Q_PROCESSOR_X86_64
#    define CFDESKTOP_ARCH_X86_64
#endif
#ifdef Q_PROCESSOR_ARM_64
#    define CFDESKTOP_ARCH_ARM64
#endif

#ifdef Q_PROCESSOR_ARM_32
#    define CFDESKTOP_ARCH_ARM32
#endif
