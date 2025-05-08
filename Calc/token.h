#pragma once
/*
 * Copyright 2023-2025 Solomina Elle, a.rainman on gmail point com
 */

#include "identifiers.h"

struct Token
{
	enum class Type : ParamCount
	{
		// https://en.cppreference.com/w/cpp/language/ascii
		END = 0x00,
		ADD = '+',
		SUB = '-',
		MUL = '*',
		DIV = '/',
		POW = '^',
		REM = '%',
		LPAREN = '(',
		RPAREN = ')',
		COMA = ',',
		NUM = '0',
		FUNCT = 'a',
		ERROR = 0x7F,
	};

	union
	{
		// If token is a function, there is a pointer to it.
		Identifiers::map::const_pointer func; //-V117

		// If token is a number parsed from string or a constant, this is the value of it.
		Value val;
	};

	// Type of this token.
	Type type;
};