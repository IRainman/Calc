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

// add headers that you want to pre-compile here

#include <array>
#include <format>
#include <limits>
#include <numbers>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>

#define FMT_UNICODE 0
#define FMT_USE_LOCALE 0
#define FMT_HEADER_ONLY 1
#define FMT_USE_FLOAT 0
#define FMT_USE_DOUBLE 1
#define FMT_USE_LONG_DOUBLE 0
// Disable C4702 for fmt because it's generate incorrect error when constexpr if is using this is an MSVC bug.
#pragma warning(push)
#pragma warning(disable: 4702)
#include <fmt/compile.h>
// Restore the previous warning state
#pragma warning(pop)

#define CALC_USING_FASTFLOAT
#ifdef CALC_USING_FASTFLOAT
	#define CALC_USING_MY_FASTFLOAT
	#ifdef CALC_USING_MY_FASTFLOAT
		#define FASTFLOAT_ONLY_POSITIVE_C_NUMBER_WO_INF_NAN
		#define FASTFLOAT_ONLY_ROUNDS_TO_NEAREST_SUPPORTED
		#include "../../fast_float/include/fast_float/fast_float.h"
/*
Tests:
 time is: 21810ms.
*/
	#else
		#include <fast_float/fast_float.h>
/*
Tests:
 time is: 24870ms.
*/
	#endif
#endif

#endif //PCH_H
