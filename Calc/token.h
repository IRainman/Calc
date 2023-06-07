//
// Prototype writed by Tamika Nomara on 28.05.2023.
//
// Full version implemented by Elle Solomina and maintained since 29.05.2023.
//

#pragma once

#include "Flags.h"

struct Token
{
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
	long double val;
};

#if 0

[[deprecated("needs to move from here to another new class, like formatter, or something like this")]]
std::ostream& operator<<(std::ostream& os, const Token::Type token);

[[deprecated("needs to move from here to another new class, like formatter, or something like this")]]
std::ostream& operator<<(std::ostream& os, const Token& token);

#endif
