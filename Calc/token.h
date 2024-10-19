#pragma once
/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

struct Token
{
	/*
	 * Type using as a type for calculation.
	 */
	using Value = long double;
	using ParamCount = unsigned short;

	enum class Type : unsigned int
	{
		END = 0,
		ADD,
		SUB,
		MUL,
		DIV,
		POW,
		REM,
		LPAREN,
		RPAREN,
		COMA,
		NUM,
		IDENT,
	};

	// Type of this token.
	Type type;

	// Text of the token.
	std::string_view text;

	// If token is a number, this is the parsed value of it.
	Value val;
};