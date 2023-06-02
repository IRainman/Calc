// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

//
// Created by Tamika Nomara on 28.05.2023.
//
//
// Manteined by Elle Solomina since 29.05.2023.
//

#include "stdafx.h"

#include "issue_manager.h"
#include "parser.h"

namespace
{
using Value = long double;

struct Fn
{
	Value(*fn)(std::span<const Value>); //-V122
	const size_t n; //-V122
};

template <const size_t I>
using WrappedFnImplArg = Value;
template <const size_t... Is>
auto WrappedFnImpl(std::index_sequence<Is...>) noexcept -> Value(*)(WrappedFnImplArg<Is>...);
template <const size_t N>
using WrappedFn = decltype(WrappedFnImpl(std::make_index_sequence<N>()));
template <typename Fn, const size_t... Is>
Value call_fn(Fn fn, std::span<const Value>& params, std::index_sequence<Is...>) noexcept
{
	return fn(params[Is]...);
}
template<const size_t N, WrappedFn<N> wrappedFn>
Value function_pointer_impl(std::span<const Value> params) noexcept
{
	return call_fn(wrappedFn, params, std::make_index_sequence<N>());
}

template<const size_t N, WrappedFn<N> wrappedFn>
constexpr Fn function_pointer() noexcept
{
	return { function_pointer_impl<N, wrappedFn>, N };
}

template <const Value value>
Value constant_impl(std::span<const Value>) noexcept
{
	return value;
}

template <const Value value>
constexpr Fn constant() noexcept
{
	return { constant_impl<value>, 0 };
}

constexpr Value min(std::span<const Value> params) noexcept
{
	return std::ranges::min(params);
}

constexpr Value max(std::span<const Value> params) noexcept
{
	return std::ranges::max(params);
}

constexpr Value rad(const Value x) noexcept { return x * std::numbers::pi_v<Value> / 180; }
constexpr Value deg(const Value x) noexcept { return x * 180 / std::numbers::pi_v<Value>; }

constexpr auto IDENTIFIERS_unlimited_params = std::numeric_limits<size_t>::max();

const std::unordered_map<std::string_view, Fn> IDENTIFIERS =
{
	//---------------------------------------------------------------------------
	// TODO https://en.cppreference.com/w/cpp/numeric/constants
	{"pi", constant<std::numbers::pi_v<Value>>()},// Pi, Archimedes' constant or Ludolph's number
	{"e", constant<std::numbers::e_v<Value>>()},// Euler's number
	{"phi", constant<std::numbers::phi_v<Value>>()},// Golden ratio
	//---------------------------------------------------------------------------
	// TODO additional constant
	{"c", constant<299792458.0L>()},// Speed of light in vacuum (m·s-1)
	{"G", constant<6.674286767676767676767676767676767676e-11L> ()}, // Newtonian constant of gravitation (m3·kg−1·s−2), this constant valid on Earths only ;)
	{"J", constant<3.058198247456354132564564787888767L>()},// Constants of Gauss fild
	{"atm", constant<101325.0L>()},// Standard atmosphere (Pa), this constant valid on Earths only ;)
	{"L", constant<6.022141510101010101010101010101010101e23L>()},// Avogadro's number (mol−1)
	{"R", constant<8.314472151515151515151515151515151515L>()},// Gas constant (J·K−1·mol−1)
	{"h", constant<6.626068963333333333333333333333333333e-34L> ()}, // Planck constant (J·s)
	//---------------------------------------------------------------------------
	// TODO https://en.cppreference.com/w/cpp/numeric/math
	// TODO https://en.cppreference.com/w/cpp/numeric/special_math
	{"sin", function_pointer<1, std::sin>()},// sinus
	{"cos", function_pointer<1, std::cos>()},// cosinus
	{"tan", function_pointer<1, std::tan>()},// tangens
	{"arccos", function_pointer<1, std::acos>()},// arccosinus
	{"arcsin", function_pointer<1, std::asin>()},// arcsinus
	{"arctan", function_pointer<1, std::atan>()},// arctangens
	
	// TODO merge this into one function
	{"hypot2", function_pointer<2, std::hypot>()},
	{"hypot3", function_pointer<3, std::hypot>()},// https://en.cppreference.com/w/cpp/numeric/math/hypot
	
	{"exp", function_pointer<1, std::exp>()},// exponent function, on x=1 return value is e
	
	{"sh", function_pointer<1, std::sinh>()},// https://en.cppreference.com/w/cpp/numeric/math/sinh
	{"ch", function_pointer<1, std::cosh>()},// https://en.cppreference.com/w/cpp/numeric/math/cosh
	{"tanh", function_pointer<1, std::tanh>()},// https://en.cppreference.com/w/cpp/numeric/math/tanh
	{"asinh", function_pointer<1, std::asinh>()},// https://en.cppreference.com/w/cpp/numeric/math/asinh
	{"acosh", function_pointer<1, std::acosh>()},// https://en.cppreference.com/w/cpp/numeric/math/acosh
	{"atanh", function_pointer<1, std::atanh>()},// https://en.cppreference.com/w/cpp/numeric/math/atanh
	
	// TODO merge this into one function
	{"log", function_pointer<1, std::log>()},// natural logarithm
	{"log10", function_pointer<1, std::log10>()},// base-10 logarithm
	{"log2", function_pointer<1, std::log2>()},// https://en.cppreference.com/w/cpp/numeric/math/log2
	{"log1p", function_pointer<1, std::log1p>()},// https://en.cppreference.com/w/cpp/numeric/math/log1p
	
	// TODO merge this into one function
	{"sqrt", function_pointer<1, std::sqrt>()},// square root https://en.cppreference.com/w/cpp/numeric/math/sqrt
	{"cbrt", function_pointer<1, std::cbrt>()},// https://en.cppreference.com/w/cpp/numeric/math/cbrt
	{"pow", function_pointer<2, std::pow>()},// power
	
	{"rad", function_pointer<1, rad>()},// One radian is equivalent to 180/PI degrees.
	{"deg", function_pointer<1, deg>()},
	
	{"min", {min, IDENTIFIERS_unlimited_params}},
	{"max", {max, IDENTIFIERS_unlimited_params}},
	
	{"abs", function_pointer<1, abs>()},// https://en.cppreference.com/w/cpp/numeric/math/fabs
	
	{"tgamma", function_pointer<1, tgamma>()}, // https://en.cppreference.com/w/cpp/numeric/math/tgamma
	{"lgamma", function_pointer<1, lgamma>()}, // https://en.cppreference.com/w/cpp/numeric/math/lgamma
	
	{"trunc", function_pointer<1, trunc>()}, // https://en.cppreference.com/w/cpp/numeric/math/trunc
	{"round", function_pointer<1, round>()}, // https://en.cppreference.com/w/cpp/numeric/math/round
	
	//ceil,
	//fabs,
	//floor,
	//ldexp,
	//modf,
	//atan2,
	//modf,
	//fmod,
	//frexp,
	//gcd,// https://en.cppreference.com/w/cpp/numeric/gcd
	//lcm,// https://en.cppreference.com/w/cpp/numeric/lcm
	//
	//---------------------------------------------------------------------------
};
}

[[nodiscard]] long double Parser::parse()
{
	const auto result = parse_expr_4();
	if (_current.type != Token::Type::END)
	{
		IssueManager::get_instance().report_error(_lex.get_position(), std::format("extraneous input at the end of expression: {}" , _current.text));
		return NAN;
	}
	else if (IssueManager::get_instance().has_errors())
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
				IssueManager::get_instance().report_error(_lex.get_position(), std::format("expected closing paren, got {}", _current.text));
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
			IssueManager::get_instance().report_error(_lex.get_position(), std::format("unexpected {}", _current.text));
			return NAN;
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
		if (const auto i = IDENTIFIERS.find(name); i != IDENTIFIERS.end())
		{
			const auto [function, N] = i->second;
			if (!N)
			{
				IssueManager::get_instance().report_error(_lex.get_position(), std::format("identifier {} is a constant, not a function", name));
				return NAN;
			}
			
			std::vector<long double> params;
			
			while (true)
			{
				params.push_back(parse_expr_4());
				
				if (_current.type == Token::Type::RPAREN)
				{
					advance();
					break;
				}
				else if (_current.type == Token::Type::COMA)
				{
					advance();
					continue;
				}
				else
				{
					IssueManager::get_instance().report_error(_lex.get_position(), std::format("expected closing paren or coma, got {}", _current.text));
					return NAN;
				}
			}
			if (N == IDENTIFIERS_unlimited_params || N == params.size())
			{
				return function(params);
			}
			else
			{
				IssueManager::get_instance().report_error(pos_of_ident_start, std::format("for function {} expected {} paramethers, got {}", name, N, params.size()));
				return NAN;
			}
		}
		else
		{
			IssueManager::get_instance().report_error(pos_of_ident_start, std::format("unknown function {}", name));
			return NAN;
		}
	}
	else
	{
		if (const auto i = IDENTIFIERS.find(name); i != IDENTIFIERS.end())
		{
			const auto [constant, N] = i->second;
			if (!N)
			{
				return constant( {});
			}
			else
			{
				IssueManager::get_instance().report_error(pos_of_ident_start, std::format("function needs paramethers {}", name));
				return NAN;
			}
		}
		else
		{
			IssueManager::get_instance().report_error(pos_of_ident_start, std::format("unknown constant {}", name));
			return NAN;
		}
	}
}
