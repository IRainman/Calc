#pragma once

// Including SDKDDKVer.h defines the highest available Windows platform.

// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.

#include <WinSDKVer.h>

#ifdef CALC_SUPPORT_WINDOWS_XP
#define _WIN32_WINNT _WIN32_WINNT_WINXP // The minimal version of OS supported by application is Windows XP.
#elif defined(CALC_SUPPORT_WINDOWS_7_8_81)
#define _WIN32_WINNT _WIN32_WINNT_WIN7 // The minimal version of OS supported by application is Windows 7.
#else
#define _WIN32_WINNT _WIN32_WINNT_WIN10 // The minimal version of OS supported by application is Windows 10.
#endif

#include <SDKDDKVer.h>
