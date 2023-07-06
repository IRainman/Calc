#pragma once

#include "flags.h"
#include "targetver.h"

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

// https://learn.microsoft.com/en-us/cpp/atl/reference/compiler-options-macros?view=msvc-170#_atl_free_threaded

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

#define _ATL_ALL_WARNINGS

#define _AFX_ALL_WARNINGS

#define _ATL_SINGLE_THREADED

#define _ATL_NO_COM_SUPPORT

#include <afxwinappex.h>

// use functions from std
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

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
