#pragma once

// Including SDKDDKVer.h defines the highest available Windows platform.

// If you wish to build Calc for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.

#include <WinSDKVer.h>

// The minimal version of OS supported by Calc:
//#define CALC_SUPPORT_WINDOWS_XP // uncomment this line to set minimal supported Windows version to Windows XP.
//#define CALC_SUPPORT_WINDOWS_7_8_81 // uncomment this line to set minimal supported Windows version to Windows 7.
// If none of above lines are uncommented, the minimal supported Windows version is Windows 10.

#if defined(CALC_SUPPORT_WINDOWS_XP) || defined(CALC_SUPPORT_WINDOWS_7_8_81)
#define DECLSPEC_DEPRECATED_DDK
#endif

#if defined(CALC_SUPPORT_WINDOWS_7_8_81)
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#elif defined(CALC_SUPPORT_WINDOWS_XP)
#define _WIN32_WINNT _WIN32_WINNT_WINXP
#else
#define _WIN32_WINNT _WIN32_WINNT_WIN10
#endif

#include <SDKDDKVer.h>

#if(_WIN32_WINNT >= 0x0601)
#define CALC_SUPPORT_DPI_CHANGES // WM_DPICHANGED
#if(_WIN32_WINNT >= 0x0602)
#define CALC_DISABLE_LEGACY_IME // ImmDisableLegacyIME
#if(_WIN32_WINNT >= 0x0605)
#define CALC_SUPPORT_PER_WINDOW_DPI // GetDpiForWindow
#endif
#endif
#endif

/* not enabled because we use only ANSI input in GUI
#define CALC_USE_IME // Input Method Manager support for international text input.
*/