#pragma once
/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "token.h"

class Lexer
{
	using Value = Token::Value;
	using EquationSize = Token::EquationSize;
	public:
		explicit Lexer(const std::string_view data) noexcept
			: _view{ data }, _begin{ data.data() }
		{
		}
		Lexer() = delete;
		Lexer(const Lexer&) = delete;
		Lexer(Lexer&&) = default;

private:
		/**
		 * Write the next token to the link.
		 */
		void next(Token& token) noexcept;
		
		/**
		 * Return the current position.
		 */
		[[nodiscard]] EquationSize get_position() const noexcept
		{
			__assume(_view.data() - _begin >= 0);
		    return _view.data() - _begin;
		}
		
		/**
		 * Move current position n symbols further.
		 */
		void advance(EquationSize n) noexcept;

		/**
		 * Read an operator at the beginning of the input view and return size of it.
		 */
		[[nodiscard]] EquationSize read_operator(const auto type, Token& token) const noexcept;
		
		/**
		 * Read a number at the beginning of the input view and return size of it.
		 */
		[[nodiscard]] EquationSize read_number(Token& token) const noexcept;
		
		/**
		 * Read an identifier at the beginning of the input view and return size of it.
		 */
		[[nodiscard]] EquationSize read_ident(Token& token) const noexcept;

		/**
		 * Read the normal end at the beginning of the input view.
		 */
		void read_end(Token& token) const noexcept;

		/**
		 * Read an unknown symbol at the beginning of the input view and return size of it.
		 */
		[[nodiscard]] EquationSize read_unknown(Token& token) const noexcept;

		std::string_view _view;
		const std::string_view::const_pointer _begin; //-V122
		friend class Parser;
};
