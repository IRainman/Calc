#pragma once
//
// Prototype writed by Tamika Nomara on 28.05.2023.
//
// Full version implemented by Elle Solomina and maintained since 29.05.2023.
//

#include "issue_manager.h"
#include "token.h"

class Lexer
{
	public:
		Lexer(const std::string_view data) noexcept
			: _data {data}, _view {_data}
		{
		}
		
	public:
		/**
		 * Return the next token.
		 */
		[[nodiscard]] Token next();
		
		/**
		 * Return the current position.
		 */
		[[nodiscard]] size_t get_position() const noexcept
		{
		    return _view.data() - _data.data();
		}
		
	private:
		/**
		 * Move current position one symbol further.
		 */
		void advance(size_t n) noexcept;
		
		/**
		 * Create a new token at the beginning of the input view.
		 */
		[[nodiscard]] Token emit(Token::Type type, size_t n, long double val = NAN);
		
		/**
		 * Create a new token at the beginning of the input view, and move current position to the end of the token.
		 */
		[[nodiscard]] Token emit_and_advance(Token::Type type, size_t n, long double val = NAN);
		
		/**
		 * Read number at the beginning of the input view.
		 */
		[[nodiscard]] Token read_number();
		
		/**
		 * Read identifier at the beginning of the input view.
		 */
		[[nodiscard]] Token read_ident();
		
	private:
		const std::string_view _data;
		std::string_view _view;
};
