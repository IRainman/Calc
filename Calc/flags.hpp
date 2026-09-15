/*
 * Copyright 2009-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef FLAGS_HPP
#define FLAGS_HPP

//---------------------------------------------------------------------------
#ifdef _DEBUG
#define CALC_TESTS_ENABLED
#ifndef NDEBUG
#define CALC_TESTS_DEV_ENABLED // if commented test is measurement performance
#endif
//---------------------------------------------------------------------------
#ifdef CALC_TESTS_DEV_ENABLED
// TODO
// #define CALC_TEST_EQUATION_SOLVER // WIP
//---------------------------------------------------------------------------
// TODO?
#define CALC_SUPPORT_FRACTIONAL_OUTPUT // WIP
//---------------------------------------------------------------------------
// TODO?
// CALC_USED_CONDITIONS
#ifdef CALC_USED_CONDITIONS
// CALC_USED_EQUALS_OPERATORS
// CALC_USED_CYCLES
#endif
//---------------------------------------------------------------------------
#endif
#endif

#endif
