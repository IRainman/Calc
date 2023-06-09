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

namespace
{
	auto& im = IssueManager::get_instance();
};

namespace
{
using Value = long double;

struct Fn
{
	struct P
	{
		constexpr static auto unlim = std::numeric_limits<size_t>::max();
		/**
		 * Test is Fn is a constant and no needs arguments.
		 */
		constexpr bool is_constant() const
		{
			return min == 0 && max == 0;
		}
		/**
		 * Test is Fn is a function and needs arguments.
		 */
		constexpr bool is_function() const
		{
			return !is_constant();
		}
		/**
		 * Test interval of paramethers count.
		 */
		constexpr bool params_count_is_valid(const size_t count) const
		{
			return min <= count || count <= max;
		}
		const size_t min; //-V122
		const size_t max; //-V122
	} check;
	Value(*fn)(std::span<const Value>); //-V122
};

template <const size_t>
using WrappedFnImplArg = Value;

template <const size_t... Is>
consteval auto WrappedFnImpl(std::index_sequence<Is...>) noexcept -> Value(*)(WrappedFnImplArg<Is>...);

template <const size_t N>
using WrappedFn = decltype(WrappedFnImpl(std::make_index_sequence<N>()));

template <typename Fn, const size_t... Is>
constexpr auto call_fn(Fn fn, std::span<const Value> params, std::index_sequence<Is...>) noexcept
{
	return fn(params[Is]...);
}

template<const size_t N, WrappedFn<N> wrappedFn>
constexpr auto function_pointer_impl(std::span<const Value> params) noexcept
{
	return call_fn(wrappedFn, params, std::make_index_sequence<N>());
}

template<const size_t N, WrappedFn<N> wrappedFn>
consteval Fn function_pointer() noexcept
{
	return { { N, 0 }, function_pointer_impl<N, wrappedFn> };
}

template <const Value value>
constexpr auto constant_impl(std::span<const Value>) noexcept
{
	return value;
}

template <const Value value>
consteval Fn constant() noexcept
{
	return { { 0, 0 }, constant_impl<value> };
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

constexpr Value hypot(std::span<const Value> params) noexcept
{
	switch (params.size())
	{
		case 2:
			return std::hypot(params[0], params[1]);
		case 3:
			return std::hypot(params[0], params[1], params[2]);
		default:
			return NAN;
	};
}

constexpr Value log(Value x) noexcept
{
	if (1 < x && x < 2)
	{
		return std::log1p(--x);
	}
	else
	{
		return std::log(x);
	}
}

static const /*TODO constinit*/ std::unordered_map<std::string_view, const Fn> IDENTIFIERS =
{
	//---------------------------------------------------------------------------
	// TODO https://en.cppreference.com/w/cpp/numeric/constants
	{"pi", constant<std::numbers::pi>()},
	{"e", constant<std::numbers::e>()},
	{"phi", constant<std::numbers::phi>()},
	{"egamma", constant<std::numbers::egamma>()},
	//---------------------------------------------------------------------------
	// TODO additional constant
	{"c", constant<299792458.0L>()},// Speed of light in vacuum (m·s-1)
	{"G", constant<6.674286767676767676767676767676767676e-11L>()},// Newtonian constant of gravitation (m3·kg−1·s−2), this constant valid on Earths only ;)
	{"J", constant<3.058198247456354132564564787888767L>()},// Constants of Gauss fild
	{"atm", constant<101325.0L>()},// Standard atmosphere (Pa), this constant valid on Earths only ;)
	{"L", constant<6.022141510101010101010101010101010101e23L>()},// Avogadro's number (mol−1)
	{"R", constant<8.314472151515151515151515151515151515L>()},// Gas constant (J·K−1·mol−1)
	{"h", constant<6.626068963333333333333333333333333333e-34L>()},// Planck constant (J·s)
	//---------------------------------------------------------------------------
	// TODO https://en.cppreference.com/w/cpp/numeric/math
	{"sin", function_pointer<1, std::sin>()},
	{"cos", function_pointer<1, std::cos>()},
	{"tan", function_pointer<1, std::tan>()},
	{"arccos", function_pointer<1, std::acos>()},
	{"arcsin", function_pointer<1, std::asin>()},
	{"arctan", function_pointer<1, std::atan>()},
	
	{"hypot", {{2, 3}, hypot}},
	
	{"exp", function_pointer<1, std::exp>()},
	
	{"sh", function_pointer<1, std::sinh>()},
	{"ch", function_pointer<1, std::cosh>()},
	{"tanh", function_pointer<1, std::tanh>()},
	{"asinh", function_pointer<1, std::asinh>()},
	{"acosh", function_pointer<1, std::acosh>()},
	{"atanh", function_pointer<1, std::atanh>()},
	
	{"ln", function_pointer<1, log>()},
	{"log", function_pointer<1, log>()},

	{"lg", function_pointer<1, std::log10>()},
	{"log10", function_pointer<1, std::log10>()},

	{"lb", function_pointer<1, std::log2>()},
	{"log2", function_pointer<1, std::log2>()},
	
	{"sqrt", function_pointer<1, std::sqrt>()},
	{"cbrt", function_pointer<1, std::cbrt>()},
	{"pow", function_pointer<2, std::pow>()},
	
	{"rad", function_pointer<1, rad>()},
	{"deg", function_pointer<1, deg>()},
	
	{"min", {{1, Fn::P::unlim}, min}},
	{"max", {{1, Fn::P::unlim}, max}},
	
	{"abs", function_pointer<1, abs>()},
	
	{"tgamma", function_pointer<1, tgamma>()},
	{"lgamma", function_pointer<1, lgamma>()},
	
	{"trunc", function_pointer<1, trunc>()},
	{"round", function_pointer<1, round>()},
	
	//---------------------------------------------------------------------------
	// TODO https://en.cppreference.com/w/cpp/numeric/special_math
	{"riemann_zeta", function_pointer<1, std::riemann_zeta>()},
	//---------------------------------------------------------------------------
};
}

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
				im.report_error(_lex.get_position(), std::format("expected closing paren, got {}", _current.text));
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
		if (const auto i = IDENTIFIERS.find(name); i != IDENTIFIERS.end())
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
							im.report_error(pos_of_ident_start, std::format("for function {} expected minimum {} and maximum {} paramethers, got {}", name, check.min, check.max, params.size()));
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
						im.report_error(_lex.get_position(), std::format("expected closing paren or coma, got {}", _current.text));
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
		if (const auto i = IDENTIFIERS.find(name); i != IDENTIFIERS.end())
		{
			const auto [check, constant] = i->second;
			if (check.is_constant())
			{
				return constant({});
			}
			else
			{
				im.report_error(pos_of_ident_start, std::format("function {} needs paranthesis for call", name));
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
