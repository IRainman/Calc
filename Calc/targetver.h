#pragma once

/**
 * If you wish to build Calc for a previous Windows platform, include
 * winsdkver.h before including sdkddkver.h.
 */

#include <winsdkver.h>

/**
 * To set minimal version of OS supported by Calc please uncomment one of four
 * line below. If all lines are commented the minimal supported version is
 * Windows 10.
 */
// #define CALC_SUPPORT_WINDOWS_7
#ifndef CALC_SUPPORT_WINDOWS_7
// #define CALC_SUPPORT_WINDOWS_VISTA
#ifndef CALC_SUPPORT_WINDOWS_VISTA
// #define CALC_SUPPORT_WINDOWS_XP
#ifndef CALC_SUPPORT_WINDOWS_XP
// #define CALC_SUPPORT_WINDOWS_2000
#endif
#endif
#endif

#if defined(CALC_SUPPORT_WINDOWS_2000)
#define _WIN32_WINNT _WIN32_WINNT_WIN2K
#elif defined(CALC_SUPPORT_WINDOWS_XP)
#define _WIN32_WINNT _WIN32_WINNT_WINXP
#elif defined(CALC_SUPPORT_WINDOWS_VISTA)
#define _WIN32_WINNT _WIN32_WINNT_VISTA
#elif defined(CALC_SUPPORT_WINDOWS_7)
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#else
#define _WIN32_WINNT _WIN32_WINNT_WIN10
#endif

#if (_WIN32_WINNT < _WIN32_WINNT_WIN10)
#define DECLSPEC_DEPRECATED_DDK
#endif

/**
 * Including sdkddkver.h defines the highest available Windows platform.
 */
#include <sdkddkver.h>

/**
 * Removed because now Unicode is converted by Edit, EditView and Normalizer:
 * CALC_DISABLE_IME
 */
/*
 * Deprecated because overhead, only set in compile time
 */
// #define CALC_SUPPORT_WINDOWS_VERSION_CHECK

#if (_WIN32_WINNT >= 0x0501)

#define CALC_SUPPORT_LINK_WINDOW // LinkWindow, NMLINK, ShellExecuteW

#if (_WIN32_WINNT >= 0x0502)

#define CALC_SUPPORT_SET_LIMIT_TEXT // EM_LIMITTEXT message

#if (_WIN32_WINNT >= 0x0600)

#define CALC_SUPPORT_DPI_CHANGES // SetProcessDPIAware

#define CALC_SUPPORT_AUTO_RESTART // RegisterApplicationRestart

#if (_WIN32_WINNT >= 0x0605)

#define CALC_SUPPORT_DPI_FOR_WINDOW // GetDpiForWindow
/**
 * Removed because it's very difficult to implement, and for Calc it's
 * completely over-engineering and overhead:
 * CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
 * WM_DPICHANGED signal,
 * AdjustWindowRectExForDpi,
 * SetProcessDpiAwarenessContext,
 * WM_GETDPISCALEDSIZE signal
 */

#if (_WIN32_WINNT >= 0x0A00)
#if (NTDDI_VERSION >= NTDDI_WIN10_RS5)
/**
 * Allow any EOL for input because the Normalizer converts it to a space, this
 * also speedup and cleanup formatting.
 */
#define CALC_SUPPORT_EXTENDENT_STYLES // ES_EX_ALLOWEOL_ALL

#if (NTDDI_VERSION >= NTDDI_WIN10_19H1)
/**
 * Support dark mode for Windows 10 1903+.
 */
#define CALC_SUPPORT_DARK_MODE

/**
 * For testing only, not for production, this code needs to be moved to it's own
 * utility:
 */
// #define CALC_SUPPORT_DARK_MODE_TEST_WIN32_HELPER_REALIZATION

#endif
#endif
#endif
#endif
#endif
#endif
#endif
