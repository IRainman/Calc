/*
 * Copyright 2009-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef FLAGS_HPP
#define FLAGS_HPP

//---------------------------------------------------------------------------
/**
 * Types using for calculation.
 */
using EquationSize = size_t;

#if _DEBUG && __STDCPP_FLOAT128_T__ == 1
using Value = std::float128_t;
#define CALC_USE_128_BIT_FLOAT 1
#warning "WIP: Calc is using 64-bit double implementation in many places."
#else
#if __STDCPP_FLOAT64_T__ == 1
using Value = std::float64_t;
using Integer = std::int64_t;
using UInteger = std::uint64_t;
#else
using Value = double;
using Integer = std::int64_t;
using UInteger = std::uint64_t;
#endif
#define CALC_USE_128_BIT_FLOAT 0
constexpr auto huge_value_precision = 1e15;
constexpr auto small_value_precision = 1e-15;
#endif
constexpr auto output_precision = std::numeric_limits<Value>::digits10;

using ParamCount = char;
//---------------------------------------------------------------------------
// TODO
// #define CALC_USE_ERROR_TOKEN // WIP
//---------------------------------------------------------------------------
// TODO: needs to be refactored because code is very complicated:
#define CALC_ALLOW_UNICODE_IN_GUI // allow Unicode input in GUI
//---------------------------------------------------------------------------
#ifdef _DEBUG
#define CALC_TESTS_ENABLED
#ifndef NDEBUG
#define CALC_TESTS_DEV_ENABLED // if commented test is measurement performance
#endif

#ifdef CALC_TESTS_DEV_ENABLED
// TODO
// #define CALC_TEST_EQUATION_SOLVER // WIP

// TODO
#define CALC_SUPPORT_FRACTIONAL_OUTPUT // WIP
#endif

#endif
//---------------------------------------------------------------------------
// std::map<std::string_view, const Fn> // Tests: time is: 37931ms.
// std::unordered_map<std::string_view, const Fn> // Tests: time is: 31515ms.
// flat_map<std::string_view, const Fn>  // TODO: std not available, boost no
// properly compiled.
//---------------------------------------------------------------------------

#endif
