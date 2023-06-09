// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

//
// Prototype writed by Tamika Nomara on 28.05.2023.
//
// Full version implemented by Elle Solomina and maintained since 29.05.2023.
//

#include "stdafx.h"
#include "issue_manager.h"
#include "lexer.h"

Token Lexer::next()
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
					IssueManager::get_instance().report_error(get_position(), std::format("unknown character {}", cur));
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

Token Lexer::emit(Token::Type type, size_t n, long double val)
{
	return Token
	{
		type,
		_view.substr(0, n),
		val
	};
}

Token Lexer::emit_and_advance(Token::Type type, size_t n, long double val)
{
	auto token = emit(type, n, val);
	advance(n);
	return token;
}

Token Lexer::read_number()
{
	long double val;
	const auto res = std::from_chars(_view.data(), _view.data() + _view.size(), val);
	const auto n = res.ptr - _view.data();
	
	if (res.ec != std::errc{})
	{
		// result_out_of_range
		// value_too_large
		IssueManager::get_instance().report_error(get_position(), "unable to parse number");
		return emit(Token::Type::END, 0);
	}
#if 0 // TODO need fix for this code.
	if (n > std::numeric_limits<long double>::digits10)
	{
		IssueManager::get_instance().report_warning(get_position(), std::format("the number has too many digits {} the maximum supported {}, the calculation can be performed with an error", n, std::numeric_limits<long double>::digits10));
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
