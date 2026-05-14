#pragma once
/*
 * Copyright 2009-present Elle Solomina, a.rainman on gmail point com
 */

//---------------------------------------------------------------------------
/*
 * Type using as a type for calculation.
 */
// Don't need more than 65536 symbols for equation, because GUI input limit is
// 64 KiB right now. But use only size_t here because it's a CPU related
// hardware type.
using EquationSize = size_t;

#if __STDCPP_FLOAT128_T__ == 1
using Value = std::float128_t;
#define CALC_USE_128_BIT_FLOAT 1
#warning "WIP: Calc is using 64-bit double implementation in many places."
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

using ParamCount = char;
//---------------------------------------------------------------------------
// Disabling usage of subnormals reduce precision
// #define CALC_DONT_USE_SUBNORMALS
//---------------------------------------------------------------------------
#ifdef _DEBUG
#define CALC_TESTS_ENABLED
#ifndef NDEBUG
#define CALC_TESTS_DEV_ENABLED // if commented test is measurement performance
#endif
#ifdef CALC_TESTS_DEV_ENABLED
// using for test changes in my fork of the library:
#define CALC_TEST_FASTFLOAT

#define CALC_TEST_BINARY_FUNCTIONS

// Currently zmij only for test, this is a component of the future fmt and now
// it's not support output precission with it critical for the Calc.
// #define CALC_USE_ZMIJ // Tests time is: 64978ms.
// Without: Tests time is: 78336ms.
#endif

// #define CALC_USE_ERROR_TOKEN // WIP
#endif
//---------------------------------------------------------------------------
// Usage of fmt is improve performance.
// TODO: check it and use fmt::memory_buffer for reduce new/delete from string
/*
Tests:
 time is: 24744ms.
*/
//---------------------------------------------------------------------------
// std::map<std::string_view, const Fn> // Tests: time is: 37931ms.
// std::unordered_map<std::string_view, const Fn> // Tests: time is: 31515ms.
// flat_map<std::string_view, const Fn>  // TODO: std not available, boost no
// properly compiled.
//---------------------------------------------------------------------------
