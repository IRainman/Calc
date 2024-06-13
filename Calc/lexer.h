#pragma once
//
// Prototype writed by Tamika Nomara on 28.05.2023.
//
// Full version implemented by Elle Solomina and maintained since 29.05.2023.
//

#include "issue_manager.h"

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

class Lexer
{
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

		std::string_view _view;
		const std::string_view::const_pointer _begin;
};
