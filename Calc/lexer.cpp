// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "pch.h"
#include "issue_manager.h"
#include "lexer.h"

[[nodiscard]] Token Lexer::next()
{
	while (!_view.empty())
	{
		const auto& cur = _view.front();
		switch (cur)
		{
			case '+':
				return emit_and_advance(Token::Type::ADD, 1);
			case '-':
				return emit_and_advance(Token::Type::SUB, 1);
			case '*':
				return emit_and_advance(Token::Type::MUL, 1);
			case '/':
				return emit_and_advance(Token::Type::DIV, 1);
			case '^':
				return emit_and_advance(Token::Type::POW, 1);
			case '%':
				return emit_and_advance(Token::Type::REM, 1);
			case '(':
				return emit_and_advance(Token::Type::LPAREN, 1);
			case ')':
				return emit_and_advance(Token::Type::RPAREN, 1);
			case ',':
				return emit_and_advance(Token::Type::COMA, 1);
			default:
				if (std::isdigit(cur) || cur == '.')
				{
					return read_number();
				}
				else if (std::isalpha(cur) || cur == '_')
				{
					return read_ident();
				}
				else if (std::isspace(cur))
				{
					advance(1);
				}
				else
				{
					IssueManager::report_error(get_position(), std::format("unknown character {}", cur));
					return emit(Token::Type::END, 0);
				}
		}
	}
	return emit(Token::Type::END, 0);
}

void Lexer::advance(size_t n) noexcept
{
	_view.remove_prefix(n);
}

Token Lexer::emit(Token::Type type, size_t n,Value val)
{
	return Token
	{
		type,
		_view.substr(0, n),
		val
	};
}

Token Lexer::emit_and_advance(Token::Type type, size_t n,Value val)
{
	auto token = emit(type, n, val);
	advance(n);
	return token;
}

Token Lexer::read_number()
{
	Value val;
	const auto res = std::from_chars(_view.data(), _view.data() + _view.size(), val);
	const auto n = res.ptr - _view.data();
	
	if (res.ec != std::errc{})
	{
		// result_out_of_range
		// value_too_large
		IssueManager::report_error(get_position(), "unable to parse number");
		return emit(Token::Type::END, 0);
	}
#if 0 // TODO need fix for this code.
	if (n >= std::numeric_limits<Value>::digits10)
	{
		IssueManager::report_warning(get_position(), std::format("the number has too many digits {} the maximum supported {}, the calculation can be performed with an error", n, std::numeric_limits<Value>::digits10));
	}
#endif
	
	return emit_and_advance(Token::Type::NUM, n, val);
}

Token Lexer::read_ident()
{
	size_t n = 0;
	for (; n < _view.size(); ++n)
	{
		if (!(std::isalnum(_view[n]) || _view[n] == '_'))
		{
			break;
		}
	}
	
	return emit_and_advance(Token::Type::IDENT, n);
}
