#pragma once

/**
 * To set minimal version of OS supported by WinHE framework please uncomment
 * one of four line below. If all lines are commented the minimal supported
 * version is maximum from Windows SDK.
 */
// #define WinHE_SUPPORT_WINDOWS_10
#ifndef WinHE_SUPPORT_WINDOWS_10
// #define WinHE_SUPPORT_WINDOWS_7
#ifndef WinHE_SUPPORT_WINDOWS_7
// #define WinHE_SUPPORT_WINDOWS_VISTA
#ifndef WinHE_SUPPORT_WINDOWS_VISTA
// #define WinHE_SUPPORT_WINDOWS_XP
#ifndef WinHE_SUPPORT_WINDOWS_XP
// #define WinHE_SUPPORT_WINDOWS_2000
#endif
#endif
#endif
#endif

#if defined(WinHE_SUPPORT_WINDOWS_2000)
#define _WIN32_WINNT _WIN32_WINNT_WIN2K
#elif defined(WinHE_SUPPORT_WINDOWS_XP)
#define _WIN32_WINNT _WIN32_WINNT_WINXP
#elif defined(WinHE_SUPPORT_WINDOWS_VISTA)
#define _WIN32_WINNT _WIN32_WINNT_VISTA
#elif defined(WinHE_SUPPORT_WINDOWS_7)
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#elif defined(WinHE_SUPPORT_WINDOWS_10)
#define _WIN32_WINNT _WIN32_WINNT_WIN10
#endif

/// Including sdkddkver.h defines the highest available Windows platform.
#include <sdkddkver.h>
