#pragma once
/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "token.h"
#include <string_view>

class Lexer
{
	using Value = Token::Value;
	public:
		explicit Lexer(const std::string_view data) noexcept
			: _view{data}, _begin{data.data()}
		{
		}
		Lexer() = delete;
		Lexer(const Lexer&) = delete;
		Lexer(Lexer&&) = default;

		/**
		 * Write the next token to the link.
		 */
		void next(Token& token) noexcept;
		
		/**
		 * Return the current position.
		 */
		[[nodiscard]] size_t get_position() const noexcept
		{
		    return _view.data() - _begin;
		}
		
	private:
		/**
		 * Move current position n symbols further.
		 */
		void advance(const auto n) noexcept;

		/**
		 * Read an operator at the beginning of the input view and return size of it.
		 */
		[[nodiscard]] auto read_operator(const auto type, Token& token) const noexcept;
		
		/**
		 * Read a number at the beginning of the input view and return size of it.
		 */
		[[nodiscard]] auto read_number(Token& token) const noexcept;
		
		/**
		 * Read an identifier at the beginning of the input view and return size of it.
		 */
		[[nodiscard]] auto read_ident(Token& token) const noexcept;

		/**
		 * Read the normal end at the beginning of the input view.
		 */
		void read_end(Token& token) const noexcept;

		/**
		 * Read an unknown symbol at the beginning of the input view and return size of it.
		 */
		[[nodiscard]] auto read_unknown(Token& token) const noexcept;

		std::string_view _view;
		const std::string_view::const_pointer _begin; //-V122
};
