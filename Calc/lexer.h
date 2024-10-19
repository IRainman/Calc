#pragma once
/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "token.h"

class Lexer
{
	using Value = Token::Value;
	public:
		explicit Lexer(const std::string_view data) noexcept
			: _view{data}, _begin{data.data()}
		{
		}
		Lexer(const Lexer&) = delete;
		Lexer(Lexer&&) = default;

		/**
		 * Return the next token.
		 */
		[[nodiscard]] Token next();
		
		/**
		 * Return the current position.
		 */
		[[nodiscard]] size_t get_position() const noexcept
		{
		    return _view.data() - _begin;
		}
		
	private:
		/**
		 * Move current position one symbol further.
		 */
		void advance(size_t n) noexcept;
		
		/**
		 * Create a new token at the beginning of the input view.
		 */
		[[nodiscard]] Token emit(Token::Type type, size_t n, Value val = NAN);
		
		/**
		 * Create a new token at the beginning of the input view, and move current position to the end of the token.
		 */
		[[nodiscard]] Token emit_and_advance(Token::Type type, size_t n, Value val = NAN);
		
		/**
		 * Read number at the beginning of the input view.
		 */
		[[nodiscard]] Token read_number();
		
		/**
		 * Read identifier at the beginning of the input view.
		 */
		[[nodiscard]] Token read_ident();

		std::string_view _view;
		const std::string_view::const_pointer _begin; //-V122
};
