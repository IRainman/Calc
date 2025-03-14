/*
 * Copyright 2009-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */
// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#include <array>
#include <format>
#include <limits>
#include <numbers>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>

// add headers that you want to pre-compile here
#define FMT_UNICODE 0
#define FMT_EXCEPTIONS 0
#define FMT_HEADER_ONLY 1
#include <fmt/compile.h>

#define CALC_USING_FASTFLOAT
#ifdef CALC_USING_FASTFLOAT
	#define CALC_USING_MY_FASTFLOAT
	#ifdef CALC_USING_MY_FASTFLOAT
		#define FASTFLOAT_ONLY_POSITIVE_C_NUMBER_WO_INF_NAN
		#include "../../fast_float/include/fast_float/fast_float.h"
/*
Tests:
 time is: 23462ms.
*/
	#else
		#include <fast_float/fast_float.h>
/*
Tests:
 time is: 24870ms.
*/
	#endif
#endif

//#define CALC_USE_ASSUME_PRE_STANDARD
#ifdef CALC_USE_ASSUME_PRE_STANDARD
// for support attribute [[assume]] is declared in P1774
#if defined(__clang__)
#define ASSUME(expr) __builtin_assume(expr)
#elif defined(__GNUC__) && !defined(__ICC)
#define ASSUME(expr) if (expr) {} else { __builtin_unreachable(); }
#elif defined(_MSC_VER) || defined(__ICC)
#define ASSUME(expr) __assume(expr)
/*
Tests:
 time is: 25538ms.
*/
#endif
#else
#define ASSUME(expr)
/*
Tests:
 time is: 23653ms.
*/
#endif

#endif //PCH_H
