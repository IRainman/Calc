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
#include <vector>

// add headers that you want to pre-compile here
#define FMT_UNICODE 0
#define FMT_EXCEPTIONS 0
#define FMT_HEADER_ONLY 1
#include <fmt/compile.h>

#define CALC_USING_FASTFLOAT
#ifdef CALC_USING_FASTFLOAT
#define FASTFLOAT_ONLY_POSITIVE_C_NUMBER_WO_INF_NAN
#include "../../fast_float/include/fast_float/fast_float.h"
#endif

#endif //PCH_H
