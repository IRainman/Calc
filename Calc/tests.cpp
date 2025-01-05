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
#include "tests.h"
#include <chrono>

std::string calc_tests()
{
	using Value = Token::Value;

	std::array<std::pair<const std::string, const Value>, 100> tests =
	{
		// syntax errors
		std::make_pair("2 + )", std::numeric_limits<Value>::quiet_NaN()),
		{ "2 + (", std::numeric_limits<Value>::quiet_NaN() },
		{ "e(", std::numeric_limits<Value>::quiet_NaN() },
		{ "1(", std::numeric_limits<Value>::quiet_NaN() },
		{ "sin(0 ", std::numeric_limits<Value>::quiet_NaN() },
		{ "sin(", std::numeric_limits<Value>::quiet_NaN() },
		{ "1+1 1", std::numeric_limits<Value>::quiet_NaN() },
		{ "1+1;", std::numeric_limits<Value>::quiet_NaN() },

		// value parsing
		{ "1.4e-3", 0.0014 },

		// check constants
		{ "pi", std::numbers::pi_v<Value> },
		{ "e", std::numbers::e_v<Value> },
		{ "phi", std::numbers::phi_v<Value> },
		{ "egamma", std::numbers::egamma_v<Value> },
		{ "c", 299792458.0 }, // Speed of light in vacuum (m·s-1)
		{ "G", 6.6743015151515151515151515151515151e-11 }, // Newtonian constant of gravitation (m3·kg−1·s−2)
		{ "J", 3.058198247456354132564564787888767 }, // Constants of Gauss field
		{ "atm", 101.325 }, // Standard atmosphere (Pa)
		{ "L", 6.02214076e23 }, // Avogadro's number (mol−1)
		{ "R", 8.31446261815324 }, // Gas constant (J·K−1·mol−1)
		{ "h", 6.62607015e-34 }, // Planck constant (J·s)
		{ "l_P", 1.616255181818181818181818181818181818181818e-35 }, // Planck length (m)
		{ "m_P", 2.176434242424242424242424242424242424242424e-8 }, // Planck mass (kg)
		{ "T_P", 1.4167841616161616161616161616161616161616161616161616e32 }, // Planck temperature (K)
		{ "t_P", 5.391247606060606060606060606060606060606060606060e-44 }, // Planck time (s)
		{ "mu_0", 1.2566370621219191919191919191919191919e-6 }, // magnetic constant (exactly 4 pi x 10^(-7)
		{ "epsilon_0", 8.854187817620389850536563031710750260608e-12 }, // electric constant (Ohm)
		{ "Z_0", 376.7303134617706554681984004203193082686 }, // characteristic impedance of vacuum (Ohm)

		// check additional constants
		{ "e ^ pi", 23.1406926327792690057 }, // Gelfond's constant https://en.wikipedia.org/wiki/Gelfond%27s_constant
		{ "exp(pi)", 23.1406926327792690057 }, // Gelfond's constant https://en.wikipedia.org/wiki/Gelfond%27s_constant

		// logarithmic functions
		{ "ln(e)",1.0 },
		{ "log10(1000)",3.0 },
		{ "log2(8)",3.0 },
		{ "log(sh(42) + ch(42))", 42},

		// hyperbolic functions
		{ "sh(0)", 0 },
		{ "ch(0)", 1 },
		{ "tanh(1)", std::tanh(1) },

		// misc functions
		{ "abs(-42)",42.0 },
		{ "round(3.14159)",3.0 },
		{ "trunc(3.14159)",3.0 },
		{ "hypot(3,4)",5.0 },
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
		{ "pow(2,1/3)", std::pow(2.0, 1.0 / 3.0) },
		{ "pow( sin( pi / 2 ) / .001 + 24, 2 )", 1048576.0 },

		// operation priority
		{ "2 + 2", 4.0 },
		{ "2 * 2", 4.0 },
		{ "2 + 2 * 2", 6.0 },
		{ "(2 + 2) * 2", 8.0 },

		// precision
		{ "10000 / 540 * 3", 55.5555555555555555555555555555555 },
		{ "1 / 3 * 3", 1.0 },
		{ "640320 ^ 3 + 744",   2.62537412640769e+17 },
		{ "e^(pi * sqrt(163))", 2.62537412640768e+17 },
		{"245850922 / 78256779", std::numbers::pi_v<Value> }, // https://en.wikipedia.org/wiki/Pi#Approximate_value_and_digits

		// special value support
		{ "1 / 0", std::numeric_limits<Value>::infinity() },

		// trigonometric functions
		{ "sin( rad(0) )", 0.0 },
		{ "87 * tan(pi) - 7", -7.0 },
		{ "tan(-pi)", 0.0 },
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
		{ "3.5 % 2", 1.5},
		{ "7 % 3 % 2", 1.0 },
		{ "7 % 2 ^ 2", 3.0 },
		{ "6 / 3 % 2", 0.0 },
		{ "6 % 4 / 2", 1.0 },
		{ "mod(3.5, 2)", 1.5},

		// min max functions
		{ "abs(-42)", 42.0 },
		{ "round(3.14159)", 3.0 },
		{ "trunc(3.14159)", 3.0 },
		{ "hypot(3,4)", 5.0 },
		{ "hypot(1,2,2)", 3.0 },
		{ "min(1)", 1.0 },
		{ "min(1, 2, 3)", 1.0 },
		{ "max(1, 2, 3)", 3.0 },

		// hardware present
		{ "fma(2, 2, 2)", 6.0 },

		// additional special
		{ "minkowski_distance(1, 1, 2, 3)", 6.0 }
	};


	std::string output;

	const auto start = std::chrono::steady_clock::now();

#ifndef CALC_TESTS_DEV_ENABLED
	for (auto i = 0; i < 100000; ++i)
#endif
	{
		for (const auto& t : tests)
		{

			Lexer l{ t.first };
			Parser p{ l };
			const auto result = p.parse();

			if (std::isnan(result) && std::isnan(t.second))
			{
#ifdef CALC_TESTS_DEV_ENABLED
				output += "Test OK:\r\n " + t.first + " not return a result, must return nan.\r\n " + Formatter::create_summary() + "\r\n";
#endif
			}
			else if (std::abs(result - t.second) <= std::numeric_limits<Value>::epsilon())
			{
#ifdef CALC_TESTS_DEV_ENABLED
				output += "Test OK:\r\n " + t.first + " = " + Formatter::format(t.second, true) + " == " + Formatter::format(result) + ".\r\n " + Formatter::create_summary() + "\r\n";
#endif
			}
			else
			{
				output += "Test failed:\r\n " + t.first + " = " + Formatter::format(t.second, true) + " != " + Formatter::format(result) + ".\r\n " + Formatter::create_summary() + "\r\n";
			}

			IssueManager::clear();
		}
	}

	const auto end = std::chrono::steady_clock::now();
	const std::chrono::duration<double> diff = end - start;
	output += std::format("Tests {}ed!\r\n Time is: {}.\r\n", output.empty() ? "pass" : "fail", diff);
	return output;
}

#endif
