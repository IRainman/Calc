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
#warning "WIP: Calc is using 64-bit double implementation in many places."
using Integer = std::int128_t;
using UInteger = std::uint128_t;

#define CALC_USE_128_BIT_FLOAT 1
constexpr auto huge_value_precision = 1e33;
constexpr auto small_value_precision = 1e-33;
#else
#if __STDCPP_FLOAT64_T__ == 1
using Value = std::float64_t;
#else
using Value = double;
#endif
using Integer = std::int64_t;
using UInteger = std::uint64_t;

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
