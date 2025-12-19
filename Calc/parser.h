#pragma once
/*
 * Copyright 2023-2025 Solomina Elle, a.rainman on gmail point com
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
		 * expr_1 = expr_0 | '-' expr_0 | expr_0;
		 *
		 * expr_0 = '(' expr_4 ')'
		 *        | NUM
		 *        | parse_function
		 *        ;
		 *
		 * parse_function = FUNCT ( '(' parameters )?
		 *
		 * parameters = expr_4 ( ',' expr_4 )* ')'
		 *
		 */
		
	public:
		explicit Parser(Lexer& lex) noexcept : _lex(lex)
		{
			advance();
		}
		Parser() = delete;
		Parser(const Parser&) = delete;
		Parser(Parser&&) = default;
		
		/**
		 * Parse and evaluate an expression. Return nan in case of an error.
		 */
		[[nodiscard]] Value parse() noexcept;
		
	private:
		/**
		 * Move current position one token further.
		 */
		void advance() noexcept;
		
		[[nodiscard]] Value parse_expr_4() noexcept;
		
		[[nodiscard]] Value parse_expr_3() noexcept;
		
		[[nodiscard]] Value parse_expr_2() noexcept;
		
		[[nodiscard]] Value parse_expr_1() noexcept;
		
		[[nodiscard]] Value parse_expr_0() noexcept;
		
		[[nodiscard]] Value parse_function() noexcept;
		
		Lexer& _lex;
		Token _current [[indeterminate]];
};
