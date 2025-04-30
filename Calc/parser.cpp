// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "pch.h"
#include "identifiers.h"
#include "issue_manager.h"
#include "parser.h"
#include "token.h"

namespace
{
	const auto& ids = Identifiers::get();
};

[[nodiscard]] Parser::Value Parser::parse() noexcept
{
	const auto result = parse_expr_4();
	if (_current.type != Token::Type::END) [[unlikely]]
	{
		if (_current.type == Token::Type::ERROR)
		{
#ifdef CALC_USE_ERROR_TOKEN
			return error_position;
#endif
		}
		else
		{
#ifdef CALC_USE_ERROR_TOKEN
			return error_position + extraneous input;
#else
			IssueManager::report_error(_lex.get_position(), "extraneous input");
#endif
		}
	}
	return result;
}

inline void Parser::advance() noexcept
{
	 _lex.next(_current);
}

[[nodiscard]] Parser::Value Parser::parse_expr_4() noexcept
{
	auto result = parse_expr_3();
	while (true)
	{
		switch (_current.type)
		{
			case Token::Type::ADD:
				advance();
				result += parse_expr_3();
				break;
			case Token::Type::SUB:
				advance();
				result -= parse_expr_3();
				break;
			default:
				return result;
		}
	}
}

[[nodiscard]] Parser::Value Parser::parse_expr_3() noexcept
{
	auto result = parse_expr_2();
	while (true)
	{
		switch (_current.type)
		{
			case Token::Type::MUL:
				advance();
				result *= parse_expr_2();
				break;
			case Token::Type::DIV:
				advance();
				result /= parse_expr_2();
				break;
			case Token::Type::REM:
				advance();
				result = std::fmod(result, parse_expr_2());
				break;
			default:
				return result;
		}
	}
}

[[nodiscard]] Parser::Value Parser::parse_expr_2() noexcept
{
	std::array<Value, std::numeric_limits<ParamCount>::max()> values;
	
	ParamCount count = 0;
	do
	{
		values[count] = parse_expr_1();

		if (_current.type == Token::Type::POW)
		{
			advance();
		}
		else
		{
			break;
		}
	}
	while (++count != static_cast<ParamCount>(values.size()));
	
	if (count == static_cast<ParamCount>(values.size())) [[unlikely]]
	{
		IssueManager::report_error(_lex.get_position(), fmt::format(FMT_COMPILE("too many ^ in one expression, the maximum is {}"), values.size()));
		return _current.val;
	}
	
	Parser::Value& result = values[count];
	while (--count != static_cast<ParamCount>(-1))
	{
		result = Identifiers::pow(values[count], result);
	};
	return result;
}

[[nodiscard]] Parser::Value Parser::parse_expr_1() noexcept
{
	switch (_current.type)
	{
		case Token::Type::SUB:
			advance();
			return -parse_expr_0();
		default:
			return parse_expr_0();
	}
}

[[nodiscard]] Parser::Value Parser::parse_expr_0() noexcept
{
	switch (_current.type)
	{
		case Token::Type::LPAREN: [[likely]]
		{
			advance();
			const auto result = parse_expr_4();
			if (_current.type == Token::Type::RPAREN) [[likely]]
			{
				advance();
				return result;
			}
			else [[unlikely]]
			{
				IssueManager::report_error(_lex.get_position(), "expected closing parenthesis");
				return _current.val;
			}
		}
		case Token::Type::NUM: [[likely]]
		{
			const auto num = _current.val;
			advance();
			return num;
		}
		case Token::Type::IDENT: [[likely]]
		{
			return parse_function_or_constant();
		}
		default: [[unlikely]]
		{
			IssueManager::report_error(_lex.get_position(), "unexpected");
			return _current.val;
		}
	}
}

[[nodiscard]] Parser::Value Parser::parse_function_or_constant() noexcept
{
	const auto pos_of_ident_start = _lex.get_position() - _current.text.size();
	if (const auto i = ids.find(_current.text); i != ids.end()) [[likely]]
	{
		advance();
		if (_current.type == Token::Type::LPAREN) // function
		{
			advance();
			const auto [check, function] = i->second;
			if (check.is_function()) [[likely]]
			{
				std::array<Value, std::numeric_limits<ParamCount>::max()> params;
				ParamCount count = 0;
				do
				{
					params[count] = parse_expr_4();
					++count;

					switch(_current.type)
					{
						case Token::Type::RPAREN: [[likely]]
						{
							advance();
							if (check.params_count_is_valid(count)) [[likely]]
							{
								return function({params.begin(), params.begin() + count});
							}
							else [[unlikely]]
							{
								IssueManager::report_error(pos_of_ident_start, fmt::format(FMT_COMPILE("for function {} expected minimum {} and maximum {} parameters, got {}"), i->first, static_cast<uint8_t>(check.min), static_cast<uint8_t>(check.max), static_cast<uint8_t>(count)));
								return _current.val;
							}
						}
						case Token::Type::COMA: [[likely]]
						{
							advance();
							continue;
						}
						default: [[unlikely]]
						{
							IssueManager::report_error(_lex.get_position(), "expected closing parenthesis or coma");
							return _current.val;
						}
					}
				}
				while (count != static_cast<ParamCount>(params.size())); [[likely]]

				{ [[unlikely]]
					IssueManager::report_error(pos_of_ident_start, fmt::format(FMT_COMPILE("maximum supported parameters is {}"), params.size()));
					return _current.val;
				}
			}
			else [[unlikely]]
			{
				IssueManager::report_error(pos_of_ident_start, fmt::format(FMT_COMPILE("identifier {} is not a function"), i->first));
				return _current.val;
			}
		}
		else // constant
		{
			const auto [check, constant] = i->second;
			if (check.is_constant()) [[likely]]
			{
				return constant({});
			}
			else [[unlikely]]
			{
				IssueManager::report_error(pos_of_ident_start, fmt::format(FMT_COMPILE("function {} needs parenthesis for call"), i->first));
				return _current.val;
			}
		}
	}
	else [[unlikely]]
	{
		IssueManager::report_error(pos_of_ident_start, fmt::format(FMT_COMPILE("unknown identifier {}"), _current.text));
		return _current.val;
	}
}
