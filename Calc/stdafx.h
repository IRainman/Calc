
#pragma once

#include "targetver.h"

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

#define _ATL_ENABLE_PTM_WARNING

#define _AFX_ALL_WARNINGS

//#define _ATL_SINGLE_THREADED

#define _ATL_NO_COM_SUPPORT

#include <afxwinappex.h>

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
#include <cmath>
#include <limits>
#include <stack>
