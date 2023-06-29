#pragma once
/*
 * Copyright 2009-2023 Solomina Elle Leonovna, a.rainman on gmail point com
 */

//---------------------------------------------------------------------------
#define CALC_VER2
//---------------------------------------------------------------------------
#ifndef CALC_VER2
	// Deprecated because is very slow and get high value of cyclomatic complexity https://en.wikipedia.org/wiki/Cyclomatic_complexity
	//#define ENABLE_INPUT_VALIDATION // Deprecated

	#ifdef ENABLE_LOG
	//#define EXTENDENT_REPORT_OF_POSITION_IN_LOG
	#ifdef _DEBUG
	//#define ENABLE_LOG_DEBUG
	#endif // _DEBUG
	#endif // ENABLE_LOG
#endif
//---------------------------------------------------------------------------
#ifdef _DEBUG
#define CALC_TESTS_ENABLED
#endif
//---------------------------------------------------------------------------