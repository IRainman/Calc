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

//---------------------------------------------------------------------------
// Don't needs to check _MSVC_LANG it's should be set by the compiler options
static_assert(__cplusplus >= 202302L, "Calc is C++ latest-edge standard app");
//---------------------------------------------------------------------------
// My fast-float fork and Zmij uses those instructions:
#if defined(_M_IX86) || defined(__i686__) || defined(__i386__) ||              \
    defined(__i386) || defined(__X86__) || defined(_M_X64) ||                  \
    defined(__x86_64__) || defined(__amd64__)
#define _M_IX86_FP 2 // additional librarys arch helper for MSVC.
#define __SSE2__ 1   // explicitely enable SSE2 for 32 and 64 bit builds
#define __SSE3__ 1   // explicitely enable SSE3 for 32 and 64 bit builds
#if defined(_M_X64) || defined(__x86_64__) || defined(__amd64__)
#define __SSSE3__ 1  // explicitely enable SSSE3 for 64 bit builds
#define __SSE4_1__ 1 // explicitely enable SSE4.1 for 64 bit builds
#define __SSE4_2__ 1 // explicitely enable SSE4.2 for 64 bit builds
#endif
#endif
//---------------------------------------------------------------------------
// Calc compile options:
// clang-format off
#pragma warning(disable : 4365) // signed/unsigned mismatch
#pragma warning(disable : 4464) // relative include path contains '..'
#pragma warning(disable : 4514) // unreferenced inline function has been removed
#pragma warning(disable : 4623) // default constructor was implicitly defined as deleted
#pragma warning(disable : 4625) // copy constructor was implicitly defined as deleted
#pragma warning(disable : 4626) // assignment operator was implicitly defined as deleted
#pragma warning(disable : 4668) // is not defined as a preprocessor macro, replacing with '0' for 'directives'
#pragma warning(disable : 4710) // ' ': function not inlined
#pragma warning(disable : 4711) // function ' ' selected for automatic inline expansion
#pragma warning(disable : 5026) // 'x': move constructor was implicitly defined as deleted
#pragma warning(disable : 5027) // move assignment operator was implicitly defined as deleted
#pragma warning(disable : 5030) // attribute ' ' is not recognized
#pragma warning(disable : 5045) // warning for unsafe buffer usage
#pragma warning(disable : 5222) // all unscoped attribute names are reserved for future standardization
// clang-format on
//---------------------------------------------------------------------------
#include "flags.hpp"
// add headers that you want to pre-compile here:
#include <array>         // Calc and Win32 GUI
#include <limits>        // Calc
#include <new>           // GUI (Normalizer), Formatter
#include <numbers>       // Calc
#include <optional>      // Win32 GUI RegRead helper
#include <ranges>        // Calc
#include <span>          // Calc
#include <stdfloat>      // Calc
#include <string>        // Win32 GUI
#include <string_view>   // Calc
#include <unordered_map> // Calc
#ifdef CALC_TEST_EQUATION_SOLVER
#include <complex>
#endif
#ifdef CALC_TESTS_ENABLED
#include <chrono>
#endif
//---------------------------------------------------------------------------
// Zmij compile options:
// clang-format off
#if CALC_USE_128_BIT_FLOAT
#warning "128-bit float type isn't supported by zmij. The library convert any output values to 64-bit double."
#endif
// Tests time is : 81207ms. Without Tests time is : 118164ms.
#pragma warning(push)
#pragma warning(disable : 4100) // unreferenced formal parameter
#pragma warning(disable : 4189) // local variable is initialized but not referenced
#pragma warning(disable : 4324) // structure was padded due to alignment specifier
#pragma warning(disable : 4388) // 'operator': signed/unsigned mismatch
#pragma warning(disable : 4242) // 'operator': conversion from 'type_x' to 'type_y', possible loss of data
#pragma warning(disable : 4244) // conversion from 'type_x' to 'type_y', possible loss of data
#pragma warning(disable : 4554) // 'operator' : check operator precedence for possible error; use parentheses to...
#pragma warning(disable : 4390) // empty control statement has no effect
#pragma warning(disable : 4456) // declaration of 'x' hides previous local declaration
#pragma warning(disable : 4459) // declaration of 'x' hides global declaration
#pragma warning(disable : 4804) // 'operator' : unsafe use of type 'type_x' in operation
#pragma warning(disable : 4820) // 'x': 'n' bytes padding added after data member 'y'
#pragma warning(disable : 5245) // '`anonymous-namespace'x': unreferenced function with internal linkage has been removed
// clang-format on
#include "../../zmij/zmij.cc"
#pragma warning(pop)
//---------------------------------------------------------------------------
// clang-format off
#pragma warning(push)
#pragma warning(disable : 4061) // enumerator 'x' in switch of enum 'y' is not explicitly handled by a case label
#pragma warning(disable : 4574) // 'x' is defined to be 'n': did you mean to use '#if x'?
#pragma warning(disable : 4582) // constructor is not implicitly called
#pragma warning(disable : 4820) // suppress "padding added after data member" for this struct
// fmt compile options:
#define FMT_HEADER_ONLY 1
#define FMT_USE_FLOAT 0
#define FMT_USE_LONG_DOUBLE 0
#ifndef CALC_TESTS_DEV_ENABLED
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
// because we have cleaner output we can reduce i-cache pressure:
#define FMT_OPTIMIZE_SIZE 2 // Tests time is: 49612ms.
#define FMT_OS 0
#define FMT_USE_RTTI 0
#define FMT_CPP_LIB_FILESYSTEM 0 // Tests time is: 49980ms.
#define FMT_BUILTIN_TYPES 0
#define FMT_UNICODE 0
#define FMT_REDUCE_INT_INSTANTIATIONS 1
#define FMT_USE_FULL_CACHE_DRAGONBOX 0 // Tests time is: 50111ms.
#define FMT_USE_LOCALE 0
#define FMT_STATIC_THOUSANDS_SEPARATOR '\''
// used because otherwise fmt produces much larger code:
#define FMT_ENFORCE_COMPILE_STRING
// clang-format on
#include "../../fmt/include/fmt/compile.h"
#pragma warning(pop)
//---------------------------------------------------------------------------
// fast_float compile options:
// clang-format off
#pragma warning(push)
#pragma warning(disable : 4820) // suppress "padding added after data member" for this struct
#if CALC_USE_128_BIT_FLOAT
#warning "128-bit float type isn't supported by fast_float. The library convert any user input values to 64-bit double."
#endif
// https://github.com/fastfloat/fast_float/pull/307
// Tests time is: 48459ms.
#define FASTFLOAT_ONLY_POSITIVE_C_NUMBER_WO_INF_NAN
#define FASTFLOAT_ONLY_ROUNDS_TO_NEAREST_SUPPORTED
#define FASTFLOAT_ISNOT_CHECKED_BOUNDS
// Wo options Tests time is: 53278ms. Original Tests time is: 56140ms.
// clang-format on
#include "../../fast_float/include/fast_float/fast_float.h"
#pragma warning(pop)
//---------------------------------------------------------------------------
/**
 * Types using for calculation.
 */
using EquationSize = size_t;
using ParamCount = int8_t;
using Integer = std::int64_t;
using UInteger = std::uint64_t;

#if _DEBUG && __STDCPP_FLOAT128_T__ == 1
using Value = std::float128_t;
#warning "WIP: Calc is using 64-bit double implementation in many places."

#define CALC_USE_128_BIT_FLOAT 1
constexpr auto huge_value_precision = 1e33;
constexpr auto small_value_precision = 1e-33;
#else
#if __STDCPP_FLOAT64_T__ == 1
using Value = std::float64_t;
#else
using Value = double;
#endif

#define CALC_USE_128_BIT_FLOAT 0
constexpr auto huge_value_precision = 1e15;
constexpr auto small_value_precision = 1e-15;
#endif
constexpr auto output_precision = std::numeric_limits<Value>::digits10;
//---------------------------------------------------------------------------
/**
 * Stack type for result formatting
 */
using Result = std::array<char,
#ifdef CALC_TESTS_ENABLED
                          128 * 1024
#else
                          std::hardware_destructive_interference_size
#endif
                          >;
//---------------------------------------------------------------------------
#endif
