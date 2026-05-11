#pragma once

// Including SDKDDKVer.h defines the highest available Windows platform.

// If you wish to build Calc for a previous Windows platform, include
// WinSDKVer.h and set the _WIN32_WINNT macro to the platform you wish to
// support before including SDKDDKVer.h.

#include <WinSDKVer.h>

// To set minimal version of OS supported by Calc: please uncomment one line
// below:
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
// If all lines above are commented, the minimal supported version is
// Windows 10.

#if defined(CALC_SUPPORT_WINDOWS_2000) || defined(CALC_SUPPORT_WINDOWS_XP) ||  \
    defined(CALC_SUPPORT_WINDOWS_VISTA) || defined(CALC_SUPPORT_WINDOWS_7)
#define DECLSPEC_DEPRECATED_DDK
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

#include <SDKDDKVer.h>

// not enabled. Very hard to implement and not usefull with DialogBoxParam
// #define CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART

#if (_WIN32_WINNT >= 0x0500)
// This code is don't needed anymore.
// #define CALC_SUPPORT_MONITOR_API // Multimonitor API.
#if (_WIN32_WINNT >= 0x0501)
#define CALC_SUPPORT_LINK_WINDOW // LinkWindow
#if (_WIN32_WINNT >= 0x0502)
#define CALC_SUPPORT_SET_LIMIT_TEXT // EM_LIMITTEXT message
#if (_WIN32_WINNT >= 0x0600)
#define CALC_SUPPORT_DPI_CHANGES  // SetProcessDPIAware
#define CALC_SUPPORT_AUTO_RESTART // RegisterApplicationRestart,
                                  // UnregisterApplicationRestart
#if (_WIN32_WINNT >= 0x0601)
#ifdef CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
#define CALC_SUPPORT_DPI_CHANGES_SIGNAL // WM_DPICHANGED signal
#endif
#if (_WIN32_WINNT >= 0x0605)
#define CALC_SUPPORT_PER_WINDOW_DPI // GetDpiForWindow,
                                    // AdjustWindowRectExForDpi,
                                    // WM_GETDPISCALEDSIZE
#endif
#endif
#endif
#endif
#endif
#endif
