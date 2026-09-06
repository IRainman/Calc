/*
 * Copyright 2009-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef FLAGS_HPP
#define FLAGS_HPP

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
