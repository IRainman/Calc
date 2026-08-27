#pragma once

// Including sdkddkver.h defines the highest available Windows platform.

// If you wish to build Calc for a previous Windows platform, include
// winsdkver.h and set the _WIN32_WINNT macro to the platform you wish to
// support before including sdkddkver.h.

#include <winsdkver.h>

// To set minimal version of OS supported by Calc:
// please uncomment one line below:
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
// If all lines above are commented:
// the minimal supported version is Windows 10.

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

#include <sdkddkver.h>

#if (_WIN32_WINNT >= 0x0501)
#define CALC_SUPPORT_LINK_WINDOW // LinkWindow, NMLINK
#if (_WIN32_WINNT >= 0x0502)
#define CALC_SUPPORT_SET_LIMIT_TEXT // EM_LIMITTEXT message
#if (_WIN32_WINNT >= 0x0600)
#define CALC_SUPPORT_DPI_CHANGES  // SetProcessDPIAware
#define CALC_SUPPORT_AUTO_RESTART // RegisterApplicationRestart
#define CALC_SUPPORT_DARK_MODE    // WM_DWMCOLORIZATIONCOLORCHANGED
#if (_WIN32_WINNT >= 0x0605)
#define CALC_SUPPORT_DPI_FOR_WINDOW // GetDpiForWindow,
// Not enabled because very hard to implement
// #define CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART // :
// WM_DPICHANGED signal,
// AdjustWindowRectExForDpi,
// SetProcessDpiAwarenessContext,
// WM_GETDPISCALEDSIZE signal

#if (_WIN32_WINNT >= 0x0A00)
#define CALC_SUPPORT_EXTENDENT_STYLES // ES_EX_ALLOWEOL_ALL
#endif
#endif
#endif
#endif
#endif
