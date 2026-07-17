/*
 * Copyright 2009-present Elle Solomina, a.rainman on gmail point com
 */

// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for
// future builds. This also affects IntelliSense performance, including code
// completion and many code browsing features. However, files listed here are
// ALL re-compiled if any one of them is updated between builds. Do not add
// files here that you will be updating frequently as this negates the
// performance advantage.

#ifndef PCH_HPP
#define PCH_HPP

// add headers that you want to pre-compile here

// #include <algorithm> // fast_float ?
#include <array>         // Calc ang Win32 GUI
#include <limits>        // Calc
#include <new>           // Formatter
#include <numbers>       // Calc
#include <optional>      // Win32 RegRead helper.
#include <ranges>        // Calc
#include <span>          // Calc
#include <stdfloat>      // Calc
#include <string_view>   // Calc
#include <unordered_map> // Calc
// #include <numeric> // ?
// #include <flat_map> // TODO: needs to check this.

#include "flags.hpp"

#ifdef CALC_TEST_EQUATION_SOLVER
#include <complex>
#endif

#ifdef CALC_USE_ZMIJ
#include "../../zmij_orig/zmij.cc" // TODO: waiting for PR is approved https://github.com/vitaut/zmij/pull/135
#if CALC_USE_128_BIT_FLOAT
#warning                                                                       \
    "128-bit float type isn't supported by zmij. The library convert any output values to 64-bit double."
#endif
#endif

#if !defined(CALC_USE_ERROR_TOKEN) || defined(CALC_TEST_FASTFLOAT) ||          \
    defined(CALC_TESTS_DEV_ENABLED)

// FMT_OPTIMIZE_SIZE 2 because we have cleaner output.
#define FMT_OPTIMIZE_SIZE 2 // Tests time is: 49612ms.

#define FMT_HEADER_ONLY 1
#define FMT_USE_FLOAT 0
#define FMT_USE_LONG_DOUBLE 0
#if defined(CALC_USE_ZMIJ) && !defined(CALC_TESTS_DEV_ENABLED)
#define FMT_USE_DOUBLE 0
#define FMT_USE_FLOAT128 0
#else
#if CALC_USE_128_BIT_FLOAT
#define FMT_USE_DOUBLE 0
#define FMT_USE_FLOAT128 1
#else
#define FMT_USE_DOUBLE 1
#define FMT_USE_FLOAT128 0
#endif
#endif

// reduce cache pressure:
#define FMT_BUILTIN_TYPES 0 // Tests time is: 49980ms.
#define FMT_USE_EXCEPTIONS 0
#define FMT_CPP_LIB_FILESYSTEM 0
#define FMT_UNICODE 0
#define FMT_USE_FULL_CACHE_DRAGONBOX 0 // Tests time is: 50111ms.
#define FMT_USE_LOCALE 0
#define FMT_STATIC_THOUSANDS_SEPARATOR '\''
// used because otherwise fmt produces much larger code:
#define FMT_ENFORCE_COMPILE_STRING
#include "../../fmt/include/fmt/compile.h"
#endif

#if CALC_USE_128_BIT_FLOAT
#warning                                                                       \
    "128-bit float type isn't supported by fast_float. The library convert any user input values to 64-bit double."
#endif

/* Tests time is: 53278ms. */

// https://github.com/fastfloat/fast_float/pull/307
#define CALC_USING_MY_FASTFLOAT

#ifdef CALC_USING_MY_FASTFLOAT
/* Tests time is: 48459ms. */
#define FASTFLOAT_ONLY_POSITIVE_C_NUMBER_WO_INF_NAN
#define FASTFLOAT_ONLY_ROUNDS_TO_NEAREST_SUPPORTED
#define FASTFLOAT_ISNOT_CHECKED_BOUNDS
#include "../../fast_float/include/fast_float/fast_float.h"
#else
/* Tests time is: 56140ms. */
// #include "../../fast_float_orig/include/fast_float/fast_float.h"
#endif

#endif
