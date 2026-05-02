/*
 * Copyright 2009-present Elle Solomina, a.rainman on gmail point com
 */
// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here

#include <algorithm>
#include <array>
//#include <flat_map> // TODO: needs to check this.
#include <limits>
#include <new>
#include <numbers>
//#include <numeric>
#include <ranges>
#include <span>
#include <string_view>
#include <stdfloat>
#include <unordered_map>

#include "flags.hpp"

#ifdef CALC_USE_ZMIJ
#include "../../zmij_orig/zmij.cc" // TODO: waiting for PR is approved https://github.com/vitaut/zmij/pull/135
#if CALC_USE_128_BIT_FLOAT
#warning "128-bit float type isn't supported by zmij. The library convert any output values to 64-bit double."
#endif
#endif

#if !defined(CALC_USE_ERROR_TOKEN) \
  || defined(CALC_TEST_FASTFLOAT) \
 || defined (CALC_TESTS_DEV_ENABLED)

#define CALC_USING_LOCAL_FMT // TODO: waiting for vcpkg version is updated? or use local instead?

#define FMT_OPTIMIZE_SIZE 2 /* Tests time is: 49612ms. This is always needs to be enabled because we have cleaner output. */
#define FMT_BUILTIN_TYPES 0 /* Tests time is: 49980ms. */
#define FMT_HEADER_ONLY 1
#define FMT_USE_FLOAT 0
#define FMT_USE_LONG_DOUBLE 0
#if defined(CALC_USE_ZMIJ) && !defined (CALC_TESTS_DEV_ENABLED)
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
#define FMT_CPP_LIB_FILESYSTEM 0
#define FMT_UNICODE 0
#define FMT_USE_EXCEPTIONS 0
#define FMT_USE_FULL_CACHE_DRAGONBOX 0 /* Tests time is: 50111ms. */
#define FMT_USE_LOCALE 0
#define FMT_STATIC_THOUSANDS_SEPARATOR '\''
#define FMT_ENFORCE_COMPILE_STRING // Always needs to be enabled, I don't know why but in the other way it's generate much more complicated code.

#ifdef CALC_USING_LOCAL_FMT
#include "../../fmt/include/fmt/compile.h"
#else
//#include <fmt/compile.h>
#endif

#endif

#if CALC_USE_128_BIT_FLOAT
#warning "128-bit float type isn't supported by fast_float. The library convert any user input values to 64-bit double."
#endif

/* Tests time is: 53278ms. */
#define CALC_USING_MY_FASTFLOAT // TODO: waiting for PR is approved https://github.com/fastfloat/fast_float/pull/307 and vcpkg version is updated.
#ifdef CALC_USING_MY_FASTFLOAT
/* Tests time is: 48459ms. */
#define FASTFLOAT_ONLY_POSITIVE_C_NUMBER_WO_INF_NAN
#define FASTFLOAT_ONLY_ROUNDS_TO_NEAREST_SUPPORTED
#define FASTFLOAT_ISNOT_CHECKED_BOUNDS
#include "../../fast_float/include/fast_float/fast_float.h"
#else
/* Tests time is: 56140ms. */
//#include "../../fast_float_orig/include/fast_float/fast_float.h"
//#include <fast_float/fast_float.h>
#endif

#endif //PCH_H
