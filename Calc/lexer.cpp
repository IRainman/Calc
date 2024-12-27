// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "pch.h"
#include "issue_manager.h"
#include "lexer.h"
#include <charconv>
#include <format>
#include <limits>
#include <system_error>
#include "token.h"

[[nodiscard]] Token Lexer::next()
{
	while (!_view.empty())
	{
		const auto& cur = _view.front();

		if ((cur >= '0' && cur <= '9') ||
			cur == '.' /* for numbers like ".054" */ ||
			cur == 'i' /* for inf */)
		{
			return read_number();
		}
		
		if ((cur >= 'a' && cur <= 'z') ||
			(cur >= 'A' && cur <= 'Z') ||
			cur == '_' /* for identification like "_something" */)
		{
			return read_ident();
		}
		
		if (cur == '+' ||
			cur == '-' ||
			cur == '*' ||
			cur == '/' ||
			cur == '^' ||
			cur == '%' ||
			cur == '(' ||
			cur == ')' ||
			cur == ',')
		{
			return emit_and_advance(static_cast<Token::Type>(cur), 1, std::numeric_limits<Value>::quiet_NaN());
		}

		if (cur == ' ' ||
			cur == '\t' ||
			cur == '\n' ||
			cur == '\v' ||
			cur == '\f' ||
			cur == '\r') // skip any spices
		{
			advance(1);
			continue;
		}
		else
		{
			IssueManager::report_error(get_position(), std::format("unknown character {}", cur));
			break;
		}
	}
	return emit(Token::Type::END, 1, std::numeric_limits<Value>::quiet_NaN());
}

void Lexer::advance(size_t n) noexcept
{
	_view.remove_prefix(n);
}

[[nodiscard]] Token Lexer::emit(Token::Type type, size_t n, Value val) noexcept
{
	return Token
	{
		_view.substr(0, n),
		val,
		type
	};
}

[[nodiscard]] Token Lexer::emit_and_advance(Token::Type type, size_t n, Value val) noexcept
{
	auto token = emit(type, n, val);
	advance(n);
	return token;
}

[[nodiscard]] Token Lexer::read_number()
{
	Value val;
	const auto res = std::from_chars(_view.data(), _view.data() + _view.size(), val);
	const auto n = res.ptr - _view.data();
	
	if (res.ec != std::errc{})
	{
		// result_out_of_range
		// value_too_large
		IssueManager::report_error(get_position(), "value is out of range or too large");
		return emit(Token::Type::END, n, std::numeric_limits<Value>::quiet_NaN());
	}
#if 0 // TODO need fix for this code.
	if (n >= std::numeric_limits<Value>::digits10)
	{
		IssueManager::report_warning(get_position(), std::format("the number has too many digits {} the maximum supported {}, the calculation can be performed with an error", n, std::numeric_limits<Value>::digits10 - 1));
	}
#endif
	
	return emit_and_advance(Token::Type::NUM, n, val);
}

[[nodiscard]] Token Lexer::read_ident() noexcept
{
	size_t n = 1;

	for (; n < _view.size(); ++n)
	{
		if (!((_view[n] >= 'a' && _view[n] <= 'z') || (_view[n] >= 'A' && _view[n] <= 'Z') || (_view[n] >= '0' && _view[n] <= '9') || _view[n] == '_'))
		{
			break;
		}
	}

	return emit_and_advance(Token::Type::IDENT, n, std::numeric_limits<Value>::quiet_NaN());
}
