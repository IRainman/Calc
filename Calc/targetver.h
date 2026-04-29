#pragma once

// Including SDKDDKVer.h defines the highest available Windows platform.

// If you wish to build Calc for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.

#include <WinSDKVer.h>

// The minimal version of OS supported by Calc:
//#define CALC_SUPPORT_WINDOWS_7 // uncomment this line to set minimal supported Windows version to Windows 7.
#ifndef CALC_SUPPORT_WINDOWS_7
//#define CALC_SUPPORT_WINDOWS_XP // uncomment this line to set minimal supported Windows version to Windows XP.
#ifndef CALC_SUPPORT_WINDOWS_XP
//#define CALC_SUPPORT_WINDOWS_NT4 // uncomment this line to set minimal supported Windows version to Windows NT4.0.
#endif
#endif
// If none of above lines are uncommented, the minimal supported Windows version is Windows 10.

#if defined (CALC_SUPPORT_WINDOWS_NT4) \
  || defined(CALC_SUPPORT_WINDOWS_XP) \
  || defined(CALC_SUPPORT_WINDOWS_7)
#define DECLSPEC_DEPRECATED_DDK
#endif

#ifdef CALC_SUPPORT_WINDOWS_NT4
#define _WIN32_WINNT _WIN32_WINNT_NT4
#elif defined(CALC_SUPPORT_WINDOWS_XP)
#define _WIN32_WINNT _WIN32_WINNT_WINXP
#elif defined(CALC_SUPPORT_WINDOWS_7)
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#else
#define _WIN32_WINNT _WIN32_WINNT_WIN10
#endif

#include <SDKDDKVer.h>

#if(_WIN32_WINNT >= 0x040A)
#define CALC_SUPPORT_IME // Disabled because we use only ANSI input in GUI
#if(_WIN32_WINNT >= 0x0500)
#define CALC_SUPPORT_MONITOR_API // Multimonitor API
#if(_WIN32_WINNT >= 0x0501)
#define CALC_SUPPORT_LINK_WINDOW // LinkWindow
#if(_WIN32_WINNT >= 0x0601)
#define CALC_SUPPORT_DPI_CHANGES // WM_DPICHANGED
#if(_WIN32_WINNT >= 0x0605)
#define CALC_SUPPORT_PER_WINDOW_DPI // GetDpiForWindow
#endif
#endif
#endif
#endif
#endif

// not enabled. TODO: very hard to realise.
//#define CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
//
