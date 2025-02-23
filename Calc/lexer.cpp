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
	token.text = _view.substr(0, 1);
	token.type = Token::Type::END;
	token.val = std::numeric_limits<Value>::quiet_NaN();
	return 0;
}

[[nodiscard]] inline Lexer::EquationSize Lexer::read_operator(const auto type, Token& token) const noexcept
{
	token.text = _view.substr(0, 1);
	token.type = static_cast<Token::Type>(type);
	token.val = std::numeric_limits<Value>::quiet_NaN();
	return 1;
}

[[nodiscard]] inline Lexer::EquationSize Lexer::read_number(Token& token) const noexcept
{
	Value val;
	const auto res = std::from_chars(_view.data(), _view.data() + _view.size(), val);
	[[assume(res.ptr - _view.data() >= 0)]];
	const EquationSize n = res.ptr - _view.data();

	token.text = _view.substr(0, n);

	if (res.ec == std::errc{}) [[likely]]
	{
		token.type = Token::Type::NUM;
		token.val = val;
	}
	else [[unlikely]]
	{
		// result_out_of_range
		// value_too_large
		// invalid_argument
		IssueManager::report_error(get_position(), "invalid number");
		token.type = Token::Type::END;
		token.val = std::numeric_limits<Value>::quiet_NaN();
	}

	return n;
}

[[nodiscard]] inline Lexer::EquationSize Lexer::read_ident(Token& token) const noexcept
{
	EquationSize n = 1;

	for (; n < _view.size(); ++n)
	{
		if (!((_view[n] >= 'a' && _view[n] <= 'z') || (_view[n] >= 'A' && _view[n] <= 'Z') || (_view[n] >= '0' && _view[n] <= '9')
#ifdef CALC_TESTS_DEV_ENABLED
			|| _view[n] == '_'

#endif
			))
		{
			break;
		}
	}

	token.text = _view.substr(0, n);
	token.type = Token::Type::IDENT;
	token.val = std::numeric_limits<Value>::quiet_NaN();

	return n;
}

void inline Lexer::read_end(Token& token) const noexcept
{
	token.text = _view;
	token.type = Token::Type::END;
	token.val = std::numeric_limits<Value>::quiet_NaN();
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
			|| cur == '.' /* for numbers like ".054" */
			|| cur == 'i' /* for inf */
#ifdef CALC_TESTS_DEV_ENABLED
			|| cur == 'n' /* for nan */
#endif
			) [[likely]]
		{
			advance(read_number(token));
			return;
		}
		else if ((cur >= 'a' && cur <= 'z')
			|| (cur >= 'A' && cur <= 'Z')
#ifdef CALC_TESTS_DEV_ENABLED
			|| cur == '_' /* for identifications like "_something" */
#endif
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
#ifdef CALC_TESTS_DEV_ENABLED
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
			IssueManager::report_error(get_position(), std::format("unknown character {}", cur));
			advance(read_unknown(token));
			return;
		}
	}

	read_end(token);
}
