#pragma once
/*
 * Copyright 2009-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

//---------------------------------------------------------------------------
#ifdef _DEBUG
#define CALC_TESTS_ENABLED
#define CALC_TESTS_DEV_ENABLED // if commented test is measurement performance

// CALC_USE_FULL_TOKENS // deprecated
// CALC_TESTS_USE_ADDITIONAL_OPTIONS // deprecated
// ISSUE_MANAGER_HAVE_SEVERITY // deprecated
#endif
//---------------------------------------------------------------------------
// CALC_TEST_FASTFLOAT // done
/*
std::from_chars

Tests: exactly: 115, almost: 9,
 failed: 4,
 time is: 32064ms.

fast_float::from_chars

Tests:
 time is: 26684ms.

( I do additional improvements and code cleanup in PR )

Tests (100% of CPU cores is used by BOINC):
 time is: 43135ms.
Tests (50% of CPU cores is used by BOINC):
 time is: 28898ms.
Tests (BOINC is stopped):
 time is: 25459ms.

When switch FASTFLOAT_ONLY_POSITIVE_C_NUMBER_WO_INF_NAN
is applied:

Tests: exactly: 115, almost: 9,
 failed: 4,
 time is: 25947ms.

( I do additional improvements and code cleanup in PR )

Tests (100% of CPU cores is used by BOINC):
 time is: 39588ms.
Tests (50% of CPU cores is used by BOINC):
 time is: 27301ms.
Tests (BOINC is stopped):
 time is: 24597ms.
*/
//---------------------------------------------------------------------------
// Usage of fmt is improve performance.
// TODO: check it and use fmt::memory_buffer for reduse new/delete from string
/*
Tests:
 time is: 24744ms.
*/
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
//std::map<std::string_view, const Fn> // Tests: time is: 37931ms.
//std::unordered_map<std::string_view, const Fn> // Tests: time is: 31515ms.
//flat_map<std::string_view, const Fn>  // TODO: std not available, boost no properly compiled.
//---------------------------------------------------------------------------