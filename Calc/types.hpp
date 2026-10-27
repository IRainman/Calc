/*
 * Copyright 2009-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef TYPES_HPP
#define TYPES_HPP

//---------------------------------------------------------------------------
#ifdef _DEBUG
/// Calc tests. Can be enabled in release for performance measurment.
#define CALC_TESTS_ENABLED
#ifndef NDEBUG
/// if disabled it's measurment performance.
#define CALC_TESTS_DEV_ENABLED
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
//---------------------------------------------------------------------------
/**
 * It's possible to disable this larger code from Calc because Unicode input are
 * really rare and very expensive to parse.
 */
#define CALC_SUPPORT_UNICODE_INPUT
//---------------------------------------------------------------------------
#endif
