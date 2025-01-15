// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "pch.h"
#include "flags.h"
#ifdef CALC_TESTS_ENABLED
#include "lexer.h"
#include "parser.h"
#include "issue_manager.h"
#include "formatter.h"
#include "identifiers.h"
#include "tests.h"
#include "token.h"

namespace
{
	using Value = Token::Value;

	enum class result : int
	{
		bit_to_bit = 0,
		less_than_epsilon = 1,
		both_nan = 2,
		failed = -1,
	};
	constexpr static bool is_ok(result r)
	{
		return r >= static_cast<result>(0);
	}
	constexpr static  bool is_failed(result r)
	{
		return !is_ok(r);
	}
	[[nodiscard]] static auto compare(const Value a, const Value b) noexcept
	{
		if (a == b)
		{
			return result::bit_to_bit;
		}
		if (Identifiers::are_almost_equal(a, b))
		{
			return result::less_than_epsilon;
		}
		if (std::isnan(a) && std::isnan(b))
		{
			return result::both_nan;
		}
		return result::failed;
	}
}

std::string calc_tests()
{
	const std::array<std::pair<const std::string, const Value>, 108> tests =
	{
		// syntax errors
		std::make_pair("2 + )", std::numeric_limits<Value>::quiet_NaN()),
		{ "2 + (", std::numeric_limits<Value>::quiet_NaN() },
		{ "e(", std::numeric_limits<Value>::quiet_NaN() },
		{ "pi(e)", std::numeric_limits<Value>::quiet_NaN() },
		{ "pi(sin)", std::numeric_limits<Value>::quiet_NaN() },
		{ "sin + 12", std::numeric_limits<Value>::quiet_NaN() },
		{ "1(", std::numeric_limits<Value>::quiet_NaN() },
		{ "sin(0 ", std::numeric_limits<Value>::quiet_NaN() },
		{ "sin(", std::numeric_limits<Value>::quiet_NaN() },
		{ "1+1 1", std::numeric_limits<Value>::quiet_NaN() },
		{ "1+1;", std::numeric_limits<Value>::quiet_NaN() },

		// value parsing
		{ "1.4e-3", 0.0014 },

		// check constants
		// https://en.cppreference.com/w/cpp/numeric/constants 
		{ "pi",                   3.14159265358979323846264338327950288 }, // https://en.wikipedia.org/wiki/Pi_(mathematical_constant)
		{ "245850922 / 78256779", 3.14159265358979323846264338327950288 }, // https://en.wikipedia.org/wiki/Pi#Approximate_value_and_digits
		{ "e", 2.718281828459045235360287471352 }, // https://en.wikipedia.org/wiki/E_(mathematical_constant)
		{ "phi", 1.6180339887498948482045868343656381177203091798057628621 }, // https://en.wikipedia.org/wiki/Golden_ratio
		{ "egamma", 0.57721566490153286060651209008240243104215933593992 }, // https://en.wikipedia.org/wiki/Euler%27s_constant
		//---------------------------------------------------------------------------
		{ "c", 299792458.0 }, // Speed of light in vacuum (m*s^-1)
		{ "G", 6.6743015151515151515151515151515151e-11 }, // Newtonian constant of gravitation (m^3*kg^−1*s^−2)
		{ "J", 3.058198247456354132564564787888767 }, // Constants of Gauss field
		{ "atm", 101.325 }, // Standard atmosphere (Pa)
		{ "g_n", 9.80665 }, // standard acceleration of gravity (m*s^-2)
		{ "N_A", 6.02214076e23 }, // Avogadro's number (mol^−1)
		{ "k", 1.380649e-23 }, // Boltzmann constant (J*K−1)
		{ "F", 9.64853321233100184e4 }, // Faraday constant (C*mol^−1)
		{ "R", 8.31446261815324 }, // Molar gas constant (J*K^−1*mol^−1)
		{ "h", 6.62607015e-34 }, // Planck constant (J*s)
		{ "G_0", 7.748091729729729729729729729729729729729729729729729e-5 }, // Conductance quantum (S)
		{ "l_P", 1.616255181818181818181818181818181818181818e-35 }, // Planck length (m)
		{ "m_P", 2.176434242424242424242424242424242424242424e-8 }, // Planck mass (kg)
		{ "T_P", 1.4167841616161616161616161616161616161616161616161616e32 }, // Planck temperature (K)
		{ "t_P", 5.391247606060606060606060606060606060606060606060e-44 }, // Planck time (s)
		{ "mu_0", 1.2566370621219191919191919191919191919e-6 }, // magnetic constant (exactly 4 pi * 10^-7)
		{ "eps_0", 8.854187817620389850536563031710750260608e-12 }, // electric constant (Ohm) (F*m^-1)
		{ "Z_0", 376.7303134617706554681984004203193082686 }, // characteristic impedance of vacuum (Ohm)
		{ "e_0", 1.602176634e-19 }, // Elementary charge (C)
		{ "eV",  1.602176634e-19 }, // Electronvolt (J)
		{ "m_e", 9.1093837139282828282828282828282828282828282828282828282828282828e-31 }, // Electron mass (kg)
		{ "m_p", 1.67262192595525252525252525252525252525252525252525252525252525252e-27 }, // Proton mass (kg)
		{ "m_u", 1.66053906892e-27 }, // Atomic mass constant (kg)

		// check additional constants
		// Gelfond's constant https://en.wikipedia.org/wiki/Gelfond%27s_constant
		{ "e ^ pi",  23.1406926327792690057 }, 
		{ "exp(pi)", 23.1406926327792690057 },

		// logarithmic functions
		{ "ln(e)",1.0 },
		{ "log10(1000)",3.0 },
		{ "log2(8)",3.0 },
		{ "log(sh(42) + ch(42))", 42},
		{ "log1p(1e-16)", 1.0e-16 },

		// hyperbolic functions
		{ "sh(0)", 0.0 },
		{ "ch(0)", 1.0 },
		{ "tanh(0)", 0.0 },

		// misc functions
		{ "abs(-42)", 42.0 },
		{ "round(3.14159)", 3.0 },
		{ "trunc(3.14159)", 3.0 },
		{ "hypot(3,4)", 5.0 },
		{ "hypot(1,2,2)", 3.0 },
		{ "max(1,2,3)",3.0 },
		{ "min(1,2,3)",1.0 },

		// basic operations
		{ "1+2", 3.0 },
		{ "5-3", 2.0 },
		{ "4*2", 8.0 },
		{ "8/4", 2.0 },
		{ "2^3", 8.0 },
		{ "5%2", 1.0 },

		// parentheses and complex expressions
		{ "(1+2)*3", 9.0 },
		{ "2*(3+4)", 14.0 },
		{ "pow(8, 1/3)", 2.0 },
		{ "pow( sin( pi / 2 ) / .001 + 24, 2 )", 1048576.0 },
		{ "pow(exp2(32), 1/4)", 256.0 },

		// operation priority
		{ "2 + 2", 4.0 },
		{ "2 * 2", 4.0 },
		{ "2 + 2 * 2", 6.0 },
		{ "(2 + 2) * 2", 8.0 },

		// precision
		{ "10000 / 540 * 3", 55.5555555555555555555555555555555 },
		{ "1 / 3 * 3", 1.0 },

		// trigonometric precision especially with pi
		{ "87 * tan(pi) - 7", -7.0 },
		{ "tan(-pi)", 0.0 },

		// TODO very big numbers?

		// special value support
		{ "1 / 0", std::numeric_limits<Value>::infinity() },

		// trigonometric functions
		{ "sin( rad(0) )", 0.0 },
		{ "tan(inf)", std::numeric_limits<Value>::quiet_NaN() },
		{ "sin(rad(30))", 0.5 },
		{ "cos(rad(60))", 0.5 },
		{ "tan(rad(45))", 1.0 },
		{ "sin(pi/4)^2", 0.5 },

		// power and root functions
		{ "pow(2,3)", 8.0 },
		{ "sqrt(4)", 2.0 },
		{ "cbrt(27)", 3.0 },
		{ "0^0", 1.0 },
		{ "-4 ^ 2", 16.0 },
		{ "- 4 ^ 2", 16.0 },
		{ "0 - 4 ^ 2", -16.0 },
		{ "3 ^ 3 ^ 3", 7625597484987.0 },
		{ "(3 ^ 3) ^ 3", 19683.0 },
		{ "2^2^2^2", 65536.0 },

		// modulo operator
		{ "-5 % 3", -2.0 },
		{ "5 % -3", 2.0 },
		{ "-5 % -3", -2.0 },
		{ "4 % 1.5", 1.0 },
		{ "7 % 3 % 2", 1.0 },
		{ "7 % 2 ^ 2", 3.0 },
		{ "6 / 3 % 2", 0.0 },
		{ "6 % 4 / 2", 1.0 },
		{ "3.5 % 2", 1.5 },
		{ "mod(3.5, 2)", 1.5},

		// min max functions
		{ "min(1)", 1.0 },
		{ "min(1, 2, 3)", 1.0 },
		{ "max(1, 2, 3)", 3.0 },

		// hardware and library present
		{ "fma(2, 2, 2)", 6.0 },

		// additional special
		{ "minkowski_dist(1, 1, 2, 3)", 6.0 }
	};


	std::string output;
	unsigned int failed = 0;

	const auto start = std::chrono::steady_clock::now();

#ifndef CALC_TESTS_DEV_ENABLED
	for (auto i = 0; i < 100000; ++i)
#endif
	{
		for (const auto& t : tests)
		{
			Lexer l{ t.first };
			Parser p{ l };
			const auto value = p.parse();

			const auto result = compare(value, t.second);

			if (is_failed(result))
			{
				++failed;
			}
#ifdef CALC_TESTS_DEV_ENABLED
			output += std::format(
				"Test {}:\r\n"
				"{}\r\n"
				"{}\r\n"
				"{}\r\n",
				is_ok(result) ? "OK" : "failed",
				t.first,
				result == result::both_nan ?
					"not return a result, must return nan." :
					std::format(
					"return = {}\r\n"
					"and it's {} equal to expect value\r\n"
					"expect = {}\r\n"
					"output = {}",
					value,
					is_failed(result) ?
					"NOT" :
					result == result::bit_to_bit ?
					"exactly" :
					"almost",
					t.second,
					value
					),
				Formatter::create_summary());
#endif

			IssueManager::clear();
		}
	}

	const auto end = std::chrono::steady_clock::now();
	const std::chrono::duration<double> diff = end - start;
	output += std::format("Tests:\r\n passed: {},\r\n failed: {},\r\n time is: {}.\r\n", tests.size() - failed, failed, diff);
	return output;
}

#endif
