#pragma once
/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

struct Token
{
	/*
	 * Type using as a type for calculation.
	 */
	using EquationSize = size_t;
	using Value = double;
	using ParamCount = char;

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
		IDENT = 'a',
		ERROR = 0x7F,
	};

	// Text of the token.
	std::string_view text;

	// If token is a number, this is the parsed value of it.
	Value val;

	// Type of this token.
	Type type;
};