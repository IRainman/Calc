/*
 * Copyright 2009-2025 Solomina Elle, a.rainman on gmail point com
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
#include <charconv>
#include <limits>
#include <numbers>
#include <ranges>
#include <span>
#include <string_view>
#include <unordered_map>

#include "flags.h"

#define FMT_HEADER_ONLY 1
#define FMT_USE_FLOAT 0
#define FMT_USE_DOUBLE 1
#define FMT_USE_LONG_DOUBLE 0
#define FMT_USE_FLOAT128 0
#define FMT_REDUCE_INT_INSTANTIATIONS 1
#define FMT_CPP_LIB_FILESYSTEM 0
#define FMT_UNICODE 0
#define FMT_USE_EXCEPTIONS 0
#define FMT_USE_FULL_CACHE_DRAGONBOX 1
#define FMT_USE_LOCALE 0
#define FMT_STATIC_THOUSANDS_SEPARATOR '\''

#define CALC_USING_MY_FMT // TODO: waiting for vcpkg version is updated.
#ifdef CALC_USING_MY_FMT
	#include "../../fmt/include/fmt/base.h"
	#include "../../fmt/include/fmt/compile.h"
#else
	#include <fmt/base.h>
	#include <fmt/compile.h>
#endif

#define CALC_USING_MY_FASTFLOAT // TODO: waiting for PR is approved and vcpkg version is updated.
#ifdef CALC_USING_MY_FASTFLOAT
	#define FASTFLOAT_ONLY_POSITIVE_C_NUMBER_WO_INF_NAN
	#define FASTFLOAT_ONLY_ROUNDS_TO_NEAREST_SUPPORTED
	// #define FASTFLOAT_ONLY_CHAR_STRING_SUPPORTED TODO?
	#include "../../fast_float/include/fast_float/fast_float.h"
#else
	#include <fast_float/fast_float.h>
#endif

#endif //PCH_H
