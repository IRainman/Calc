// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

//
// Prototype writed by Tamika Nomara on 28.05.2023.
//
// Full version implemented by Elle Solomina and maintained since 29.05.2023.
//

#include "pch.h"
#include "issue_manager.h"
#include "parser.h"
#include "identifiers.h"

namespace
{
	auto& im = IssueManager::get_instance();
	const auto& ids = Identifiers::get();
};

[[nodiscard]] long double Parser::parse()
{
	const auto result = parse_expr_4();
	if (_current.type != Token::Type::END)
	{
		im.report_error(_lex.get_position(), std::format("extraneous input at the end of expression: {}" , _current.text));
		return NAN;
	}
	else if (im.has_errors())
	{
		return NAN;
	}
	else
	{
		return result;
	}
}

void Parser::advance()
{
	_current = _lex.next();
}

long double Parser::parse_expr_4()
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

long double Parser::parse_expr_3()
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

long double Parser::parse_expr_2()
{
	std::vector<long double> values;
	
	while (true)
	{
		values.push_back(parse_expr_1());
		
		if (_current.type == Token::Type::POW)
		{
			advance();
		}
		else
		{
			break;
		}
	}
	
	long double result = values.back();
	for (auto value: std::ranges::reverse_view(values) | std::views::drop(1))
	{
		result = std::pow(value, result);
	}
	return result;
}

long double Parser::parse_expr_1()
{
	switch (_current.type)
	{
		case Token::Type::ADD:
			advance();
			return +parse_expr_0();
		case Token::Type::SUB:
			advance();
			return -parse_expr_0();
		default:
			return parse_expr_0();
	}
}

long double Parser::parse_expr_0()
{
	switch (_current.type)
	{
		case Token::Type::LPAREN:
		{
			advance();
			const auto result = parse_expr_4();
			if (_current.type == Token::Type::RPAREN)
			{
				advance();
				return result;
			}
			else
			{
				im.report_error(_lex.get_position(), std::format("expected closing parenthesis, got {}", _current.text));
				return NAN;
			}
		}
		case Token::Type::NUM:
		{
			const auto num = _current.val;
			advance();
			return num;
		}
		case Token::Type::IDENT:
		{
			return parse_function_or_constant();
		}
		default:
		{
			im.report_error(_lex.get_position(), std::format("unexpected {}", _current.text));
			return NAN;
		}
	}
}

long double Parser::parse_function_or_constant()
{
	const auto name = _current.text; //-V836
	const auto pos_of_ident_start = _lex.get_position();
	advance();
	if (_current.type == Token::Type::LPAREN)
	{
		advance();
		if (const auto i = ids.find(name); i != ids.end())
		{
			const auto [check, function] = i->second;
			if (check.is_function())
			{
				std::vector<long double> params;

				while (true)
				{
					params.push_back(parse_expr_4());

					if (_current.type == Token::Type::RPAREN)
					{
						advance();
						if (check.params_count_is_valid(params.size()))
						{
							return function(params);
						}
						else
						{
							im.report_error(pos_of_ident_start, std::format("for function {} expected minimum {} and maximum {} parameters, got {}", name, check.min, check.max, params.size()));
							return NAN;
						}
					}
					else if (_current.type == Token::Type::COMA)
					{
						advance();
						continue;
					}
					else
					{
						im.report_error(_lex.get_position(), std::format("expected closing parenthesis or coma, got {}", _current.text));
						return NAN;
					}
				}
			}
			else
			{
				im.report_error(pos_of_ident_start, std::format("identifier {} is not a function", name));
				return NAN;
			}
		}
		else
		{
			im.report_error(pos_of_ident_start, std::format("unknown function {}", name));
			return NAN;
		}
	}
	else
	{
		if (const auto i = ids.find(name); i != ids.end())
		{
			const auto [check, constant] = i->second;
			if (check.is_constant())
			{
				return constant({});
			}
			else
			{
				im.report_error(pos_of_ident_start, std::format("function {} needs parenthesis for call", name));
				return NAN;
			}
		}
		else
		{
			im.report_error(pos_of_ident_start, std::format("unknown constant {}", name));
			return NAN;
		}
	}
}
