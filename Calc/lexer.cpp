// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "pch.h"
#include "issue_manager.h"
#include "lexer.h"

namespace
{
	const auto& ids = Identifiers::get();
};

[[nodiscard]] EquationSize Lexer::get_position() const noexcept
{
	[[assume(_view.data() - _begin >= 0)]];
	return static_cast<EquationSize>(_view.data() - _begin);
}

inline void Lexer::advance(EquationSize n) noexcept
{
	_view.remove_prefix(n);
}

[[nodiscard]] EquationSize Lexer::read_unparsable(Token& token) const noexcept
{
	IssueManager::report_error(get_position(), "unparsable");
	token.type = Token::Type::ERROR;
	return 0;
}

[[nodiscard]] inline EquationSize Lexer::read_operator(Token& token) const noexcept
{
	[[assume((_view.size() >= 1))]];
	token.type = static_cast<Token::Type>(_view.front());
	return 1;
}

[[nodiscard]] inline EquationSize Lexer::read_number(Token& token) const noexcept
{
	const auto begin = _view.data();
	const auto end = _view.data() + _view.size();
	[[assume(end - begin >= 1)]];
#ifdef CALC_USING_FASTFLOAT
	const auto opt = fast_float::parse_options{fast_float::chars_format::general
#ifndef CALC_USING_MY_FASTFLOAT
		 | fast_float::chars_format::no_infnan
#endif
	};
	const auto res = fast_float::from_chars_advanced(begin, end, token.val, opt);
#else // use std
	const auto res = std::from_chars(begin, end, token.val);
#endif
	[[assume(res.ptr - begin >= 1)]];
	const auto n = static_cast<EquationSize>(res.ptr - begin);
	if (res.ec == std::errc{}) [[likely]]
	{
		[[assume(token.val >= 0 && token.val <= std::numeric_limits<Value>::max() && !std::isnan(token.val))]];
		token.type = Token::Type::NUM;
		return n;
	}
	
	[[unlikely]]
	return read_unparsable(token);
}

[[nodiscard]] inline EquationSize Lexer::read_ident(Token& token) const noexcept
{
	[[assume((_view.size() >= 1))]];
	EquationSize n = 1;

	while (n != _view.size())
	{
		if (!((_view[n] >= 'a' && _view[n] <= 'z') || (_view[n] >= 'A' && _view[n] <= 'Z') || (_view[n] >= '0' && _view[n] <= '9') || _view[n] == '_'))
		{
			break;
		}
		++n;
	}

	if (const auto i = ids.find(_view.substr(0, n)); i != ids.end()) [[likely]]
	{
		const auto [check, constant] = i->second;
		if (check.is_constant())
		{
			token.val = constant({});
			token.type = Token::Type::NUM;
		}
		else
		{
			token.func = &(*i); // Use the address of the iterator's dereferenced value
			token.type = Token::Type::FUNCT;
		}
		return n;
	}
	
	[[unlikely]]
	return read_unparsable(token);
}

inline void Lexer::read_end(Token& token) const noexcept
{
	token.type = Token::Type::END;
}

void Lexer::next(Token& token) noexcept
{
	while (!_view.empty()) [[likely]]
	{
		// https://en.cppreference.com/w/cpp/language/ascii
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
			advance(read_operator(token));
			return;
			
		}
		if ((cur >= '0' && cur <= '9')
			) [[likely]]
		{
			advance(read_number(token));
			return;
		}
		if ((cur >= 'a' && cur <= 'z')
			|| (cur >= 'A' && cur <= 'Z')
			) [[likely]]
		{
			advance(read_ident(token));
			return;
		}
		if (cur == ' ') [[likely]]
		{
			advance(1);
			continue;
		}
#ifdef CALC_TESTS_USE_ADDITIONAL_OPTIONS
		if (cur == '\t'
			|| cur == '\n'
			|| cur == '\v'
			|| cur == '\f'
			|| cur == '\r'
			) [[unlikely]]
		{
			advance(1);
			continue;
		}
#endif

		[[unlikely]]
		advance(read_unparsable(token));
		return;
	}

	read_end(token);
}
