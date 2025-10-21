#pragma once
/*
 * Copyright 2009-2025 Solomina Elle, a.rainman on gmail point com
 */

//---------------------------------------------------------------------------
/*
 * Type using as a type for calculation.
 */
/*
In 64 bit build:

uint32_t
Tests time is: 50430ms.

size_t
Tests time is: 49737ms.
*/
using EquationSize = size_t; // TODO needs to rewrite usage in the containers.
using Value = double;
using ParamCount = char;
//---------------------------------------------------------------------------
#ifdef _DEBUG
#define CALC_TESTS_ENABLED
#define CALC_TESTS_DEV_ENABLED // if commented test is measurement performance
//#define CALC_TEST_FASTFLOAT

//#define CALC_USE_ERROR_TOKEN // WIP
// CALC_USE_FULL_TOKENS // deprecated
// CALC_TESTS_USE_ADDITIONAL_OPTIONS // deprecated
// ISSUE_MANAGER_HAVE_SEVERITY // deprecated
#endif
//---------------------------------------------------------------------------
// Usage of fmt is improve performance.
// TODO: check it and use fmt::memory_buffer for reduce new/delete from string
/*
Tests:
 time is: 24744ms.
*/
//---------------------------------------------------------------------------
#ifndef CALC_USE_ERROR_TOKEN
//#define CALC_USING_STATIC_VECTOR
// TODO: boost is working but without options and noexcept
// maybe better to use inplace_vector when it's available,
// and drop boost completely
/*
 Tests: exactly: 115, almost: 9,
 failed: 4,
 time is: 31949ms.
 
 std::vector<Issue> + reserve(10)
*/

/*
Tests: exactly: 115, almost: 9,
 failed: 4,
 time is: 31621ms.
 
 boost::container::static_vector<Issue, 10>
*/
constexpr size_t CALC_MAX_ISSUES = 10;
#endif
//---------------------------------------------------------------------------
//std::map<std::string_view, const Fn> // Tests: time is: 37931ms.
//std::unordered_map<std::string_view, const Fn> // Tests: time is: 31515ms.
//flat_map<std::string_view, const Fn>  // TODO: std not available, boost no properly compiled.
//---------------------------------------------------------------------------