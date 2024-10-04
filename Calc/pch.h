#pragma once

#include "flags.h"
#include "targetver.h"

#include <string>
#include <string_view>
#include <charconv>
#include <numbers>
#include <unordered_map>
#include <cmath>
#include <limits>
#include <stack>
#include <ranges>
#include <vector>
#include <format>
#include <span>

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _ATL_CSTRING_NO_CRT
#define _ATL_ALL_WARNINGS
#define _ATL_SINGLE_THREADED
#define _ATL_NO_COM_SUPPORT
#define _AFX_MINREBUILD
#define _AFX_ALL_WARNINGS
#define _AFXDLL
#include <afxwinappex.h>

// use functions from std
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif
