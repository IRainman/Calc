// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "pch.h"
#include "issue_manager.h"
#include "lexer.h"
#include "token.h"

[[nodiscard]] Lexer::EquationSize Lexer::get_position() const noexcept
{
	[[assume(_view.data() - _begin >= 0)]];
	return _view.data() - _begin;
}

inline void Lexer::advance(Lexer::EquationSize n) noexcept
{
	_view.remove_prefix(n);
}

[[nodiscard]] inline Lexer::EquationSize Lexer::read_unknown(Token& token) const noexcept
{
	token.type = Token::Type::ERROR;
	return 0;
}

[[nodiscard]] inline Lexer::EquationSize Lexer::read_operator(const auto type, Token& token) const noexcept
{
	token.type = static_cast<Token::Type>(type);
	return 1;
}

[[nodiscard]] inline Lexer::EquationSize Lexer::read_number(Token& token) const noexcept
{
	[[assume(_view._Unchecked_end() - _view._Unchecked_begin() >= 1)]];
#ifdef CALC_USING_FASTFLOAT
	const auto res = fast_float::from_chars(_view._Unchecked_begin(), _view._Unchecked_end(), token.val);
#else
	const auto res = std::from_chars(_view._Unchecked_begin(), _view._Unchecked_end(), token.val);
#endif
	[[assume(res.ptr - _view._Unchecked_begin() >= 1)]];
	const EquationSize n = res.ptr - _view._Unchecked_begin();
	if (res.ec == std::errc{}) [[likely]]
	{
		[[assume(token.val >= 0 && token.val <= std::numeric_limits<Value>::max() && !std::isnan(token.val))]];
		token.type = Token::Type::NUM;
	}
	else [[unlikely]]
	{
		// result_out_of_range
		// value_too_large
		// value_too_small
		// invalid_argument
		IssueManager::report_error(get_position(), "invalid number");
		token.type = Token::Type::ERROR;
	}

	return n;
}

[[nodiscard]] inline Lexer::EquationSize Lexer::read_ident(Token& token) const noexcept
{
	[[assume((_view.size() >= 1)]];
	EquationSize n = 1;

	for (; n < _view.size(); ++n)
	{
		if (!((_view[n] >= 'a' && _view[n] <= 'z') || (_view[n] >= 'A' && _view[n] <= 'Z') || (_view[n] >= '0' && _view[n] <= '9') || _view[n] == '_'))
		{
			break;
		}
	}

	token.text = _view.substr(0, n);
	token.type = Token::Type::IDENT;

	return n;
}

inline void Lexer::read_end(Token& token) const noexcept
{
	token.type = Token::Type::END;
}

void Lexer::next(Token& token) noexcept
{
	while (!_view.empty()) [[likely]]
	{
		const auto& cur = _view.front();
		if (cur == '+' ||
			cur == '-' ||
			cur == '*' ||
			cur == '/' ||
			cur == '^' ||
			cur == '%' ||
			cur == '(' ||
			cur == ')' ||
			cur == ','
			) [[likely]]
		{
			advance(read_operator(cur, token));
			return;
			
		}
		else if ((cur >= '0' && cur <= '9')
			) [[likely]]
		{
			advance(read_number(token));
			return;
		}
		else if ((cur >= 'a' && cur <= 'z')
			|| (cur >= 'A' && cur <= 'Z')
			) [[likely]]
		{
			advance(read_ident(token));
			return;
		}
		else if (cur == ' ') [[likely]]
		{
			// skip any spices
			advance(1);
			continue;
		}
#ifdef CALC_TESTS_USE_ADDITIONAL_OPTIONS
		// https://en.cppreference.com/w/cpp/language/ascii
		else if (cur == 0xA0
			|| cur == '\t'
			|| cur == '\n'
			|| cur == '\v'
			|| cur == '\f'
			|| cur == '\r'
			) [[unlikely]]
		{
			// skip any spices
			advance(1);
			continue;
		}
#endif
		else [[unlikely]]
		{
			IssueManager::report_error(get_position(), fmt::format(FMT_COMPILE("unknown character {}"), cur));
			advance(read_unknown(token));
			return;
		}
	}

	read_end(token);
}
