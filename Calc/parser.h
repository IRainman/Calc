#pragma once
/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "lexer.h"

class Parser
{
		/*
		 * This is a recursive descent parser with the following grammar:
		 *
		 * expr   = expr_4 <END>
		 *
		 * expr_4 = expr_3 ( ('+' | '-' ) expr_3 )*;
		 *
		 * expr_3 = expr_2 ( ('*' | '/' | '%') expr_2 )*;
		 *
		 * expr_2 = expr_1 ( '^' expr_1 )*;
		 *
		 * expr_1 = expr_0 | '-' expr_0 | '+' expr_0;
		 *
		 * expr_0 = '(' expr_4 ')'
		 *        | NUM
		 *        | function_or_constant
		 *        ;
		 *
		 * function_or_constant = IDENT ( '(' function_params )?
		 *
		 * function_params = expr_4 ( ',' expr_4 )* ')'
		 *
		 */
		
	public:
		explicit Parser(Lexer& lex) : _lex(lex)
		{
			advance();
		}
		Parser(const Parser&) = delete;
		Parser(Parser&&) = default;
		
		/**
		 * Parse and eval an expression. Return nan in case of an error.
		 */
		[[nodiscard]] long double parse();
		
	private:
		/**
		 * Move current position one token further.
		 */
		void advance();
		
		[[nodiscard]] long double parse_expr_4();
		
		[[nodiscard]] long double parse_expr_3();
		
		[[nodiscard]] long double parse_expr_2();
		
		[[nodiscard]] long double parse_expr_1();
		
		[[nodiscard]] long double parse_expr_0();
		
		[[nodiscard]] long double parse_function_or_constant();
		
		Lexer& _lex;
		Token _current;
};
