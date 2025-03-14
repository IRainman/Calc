// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "pch.h"
#include "flags.h"
#ifdef CALC_TESTS_ENABLED
#include <chrono>
#include "lexer.h"
#include "parser.h"
#include "issue_manager.h"
#ifdef CALC_TESTS_DEV_ENABLED
#include "formatter.h"
#endif
#include "identifiers.h"
#include "tests.h"
#include "token.h"

#ifdef CALC_TEST_FASTFLOAT
std::string test_fast_float_parsing()
{
	unsigned int bin_val, hex_val;
	float f_val;
	constexpr std::string_view data_bin = "1010010010000011001001010101001";
	auto res = fast_float::from_chars(data_bin._Unchecked_begin(), data_bin._Unchecked_end(), bin_val, 2);
	constexpr std::string_view data_hex = "abCdEf69";
	res = fast_float::from_chars(data_hex._Unchecked_begin(), data_hex._Unchecked_end(), hex_val, 16);
	constexpr std::string_view data_float = "12345678e-9";
	res = fast_float::from_chars(data_float._Unchecked_begin(), data_float._Unchecked_end(), f_val);
	return fmt::format(FMT_COMPILE("bin:{}: {},\r\nhex:{}: {},\r\nfloat:{}: {}"), data_bin, bin_val, data_hex, hex_val, data_float, f_val);
}
#endif

namespace
{
	using Value = Token::Value;

	enum class result : char
	{
		failed = -1,

		bit_to_bit = 0,
		less_than_epsilon = 2,
		has_no_value = 1,
	};
	[[nodiscard]] constexpr bool is_ok(result r)
	{
		return r >= static_cast<result>(0);
	}
	[[nodiscard]] constexpr bool is_failed(result r)
	{
		return r < static_cast<result>(0);
	}
	[[nodiscard]] constexpr bool is_less_than_epsilon(result r)
	{
		return r == result::less_than_epsilon;
	}
	[[nodiscard]] constexpr bool has_no_value(result r)
	{
		return r == result::has_no_value;
	}
	[[nodiscard]] static inline auto compare(const Value a, const Value b) noexcept
	{
		if (a == b) //-V550
		{
			return result::bit_to_bit;
		}
		if (Identifiers::compare(a, b))
		{
			return result::less_than_epsilon;
		}
		if (std::isnan(a) && std::isnan(b))
		{
			return result::has_no_value;
		}
		return result::failed;
	}
}

std::string calc_tests()
{
	constexpr auto tests = std::to_array<std::pair<std::string_view, Value>>
	({
		// syntax errors
		{ "2 + )", std::numeric_limits<Value>::quiet_NaN() },
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
		{ "+1.4e-3", std::numeric_limits<Value>::quiet_NaN() },

		// very big numbers? No, it's should return NAN.
		{ "9999999999999999999e1000000000000000000000000000000", std::numeric_limits<Value>::quiet_NaN() },
		{ "999999999999999999e-1000000000000000000000000000000", std::numeric_limits<Value>::quiet_NaN() },

		// check constants
		// https://en.cppreference.com/w/cpp/numeric/constants 
		{ "pi",                   3.14159265358979323846264338327950288 }, // https://en.wikipedia.org/wiki/Pi_(mathematical_constant)
		{ "245850922 / 78256779", 3.14159265358979323846264338327950288 }, // https://en.wikipedia.org/wiki/Pi#Approximate_value_and_digits
		{ "e", 2.718281828459045235360287471352 }, // https://en.wikipedia.org/wiki/E_(mathematical_constant)
		{ "phi", 1.6180339887498948482045868343656381177203091798057628621 }, // https://en.wikipedia.org/wiki/Golden_ratio
		{ "egamma", 0.57721566490153286060651209008240243104215933593992 }, // https://en.wikipedia.org/wiki/Euler%27s_constant
		{ "log2(e)", std::numbers::log2e_v<Value> },
		{ "log10(e)", std::numbers::log10e_v<Value> },
		{ "1 / pi", std::numbers::inv_pi_v<Value> },
		{ "1 / sqrt(pi)", std::numbers::inv_sqrtpi_v<Value> },
		{ "ln(2)", std::numbers::ln2_v<Value> },
		{ "ln(10)", std::numbers::ln10_v<Value> },
		{ "sqrt(2)", std::numbers::sqrt2_v<Value> },
		{ "sqrt(3)", std::numbers::sqrt3_v<Value> },
		{ "1 / sqrt(3)", std::numbers::inv_sqrt3_v<Value> },

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
		{ "e ^ pi",  23.14069263277926900572 }, 
		{ "exp(pi)", 23.14069263277926900572 },

		// check long mantissa to produce correct output without noise
		{ "0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000001234567890123456789012345", 0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000001234567890123456789012345 },
		{ "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890.123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890", 123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890.123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890 },

		// scientific notation
		{ "1.4e-3", 0.0014 },
		{ "-1.4e+3", -1400.0 },

		// basic operations
		{ "1+2", 3.0 },
		{ "5-3", 2.0 },
		{ "4*2", 8.0 },
		{ "8/4", 2.0 },
		{ "2^3", 8.0 },
		{ "5%2", 1.0 },

		// advanced functions
		{ "exp(0)", 1.0 },
		{ "log1p(1e-16)", 1.0e-16 },
		{ "expm1(1e-16)", 1.0e-16 },
		{ "tgamma(5)", 24.0 },  // 4! = 24
		//{ "lgamma(5)", std::log(24) },
		{ "beta(2, 2)", 0.1666666666666666 },
		{ "erf(0)", 0.0 },
		{ "erfc(0)", 1.0 },

		// logarithmic functions
		{ "ln(e)",1.0 },
		{ "log10(1000)",3.0 },
		{ "log2(8)",3.0 },
		{ "log(sh(42) + ch(42))", 42},

		// hyperbolic functions
		{ "sh(0)", 0.0 },
		{ "ch(0)", 1.0 },
		{ "tanh(0)", 0.0 },

		// misc functions
		{ "abs(-42)", 42.0 },
		{ "round(3.14159)", 3.0 },
		{ "round(e^pi-pi)", 20.0},
		{ "trunc(3.14159)", 3.0 },
		{ "trunc(e^pi-pi)", 19.0},
		{ "hypot(3,4)", 5.0 },
		{ "hypot(1,2,2)", 3.0 },

		// operation priority
		{ "2 + 2", 4.0 },
		{ "2 * 2", 4.0 },
		{ "2 + 2 * 2", 6.0 },
		{ "(2 + 2) * 2", 8.0 },

		// parentheses and complex expressions
		{ "(1+2)*3", 9.0 },
		{ "2*(3+4)", 14.0 },
		{ "(2 + 2) * 2", 8.0 },
		{ "2 + (2 * 2)", 6.0 },
		{ "(2 + 3) * (4 - 1)", 15.0 },
		{ "pow( sin( pi / 2 ) / 0.001 + 24, 2 )", 1048576.0 },
		{ "pow(exp2(32), 1/4)", 256.0 },
		{ "sqrt(cos(rad(30))^2+sin(rad(30))^2)", 1.0 },

		// precision
		{ "10000 / 540 * 3", 55.5555555555555555555555555555555 },
		{ "500 / 9", 55.5555555555555555555555555555555 },
		{ "1 / 3 * 3", 1.0 },
		{ "9876543.210123456789", 9876543.210123456789 },
		{ "0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1", 1.0 },
		{ "100.0 - 99.99", 0.01 },
		{ "10.0 - 9.99", 0.01 },
		{ "0.2 + 0.1", 0.3 },
		{ "10.0 - 0.93", 9.07 },

		// trigonometric precision especially with pi
		{ "87 * tan(pi) - 7", -7.0 },
		{ "tan(-pi)", 0.0 },

		// special value support
		{ "1 / 0", std::numeric_limits<Value>::infinity() },

		// trigonometric functions
		{ "rad(180)", 3.14159265358979323846264338327950288 },
		{ "deg(pi)", 180.0 },
		{ "sin(0)", 0.0 },
		{ "cos(0)", 1.0 },
		{ "tan(0)", 0.0 },
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
		{ "0 - 4 ^ 2", -16.0 },
		{ "3 ^ 3 ^ 3", 7625597484987.0 },
		{ "(3 ^ 3) ^ 3", 19683.0 },
		{ "2^2^2^2", 65536.0 },

		// min max functions
		{ "min(1)", 1.0 },
		{ "min(1, 2, 3)", 1.0 },
		{ "max(1, 2, 3)", 3.0 },

		// hardware and library present
		{ "fma(2, 2, 2)", 6.0 },

		// additional special
		{ "distance(1, 1, 2, 3)", 6.0 }
	});


	std::string output;
#ifdef CALC_TESTS_DEV_ENABLED
	output.reserve(60000);
	size_t failed = 0;
	size_t almost = 0;
#endif

	const auto start = std::chrono::steady_clock::now();

#ifndef CALC_TESTS_DEV_ENABLED
	for (unsigned int i = 1'000'000; i; --i)
#endif
	{
		for (const auto& t : tests)
		{
			Lexer l(t.first);

			Parser p(l);

			const auto value = p.parse();

			[[maybe_unused]] const auto result = IssueManager::has_errors() ? result::has_no_value : compare(value, t.second);

#ifdef CALC_TESTS_DEV_ENABLED
			if(is_less_than_epsilon(result))
			{
				++almost;
			}
			else if (is_failed(result))
			{
				++failed;
			}			
			output += std::format(
				"Test {}:\r\n"
				"{}\r\n"
				"{}\r\n"
				"{}\r\n",
				is_ok(result) ? "OK" : "failed",
				t.first,
				has_no_value(result) ?
					"not return a result, must return error." :
					std::format(
					"return = {}\r\n"
					"and it's {} equal to expect value\r\n"
					"expect = {}\r\n"
					"output = {}",
					value,
					is_failed(result) ?
					"NOT" :
					is_less_than_epsilon(result) ?
					"almost" :
					"exactly",
					t.second,
					Formatter::format(value)
					),
				Formatter::create_summary());
#endif

			IssueManager::clear();
		}
	}

	const auto end = std::chrono::steady_clock::now();

	output += std::format("Tests:"
#ifdef CALC_TESTS_DEV_ENABLED
		" exactly: {}, almost: {},\r\n failed: {},"
#endif
		"\r\n time is: {}.",
#ifdef CALC_TESTS_DEV_ENABLED
		tests.size() - (failed + almost), almost, failed,
#endif
		std::chrono::duration_cast<std::chrono::
#ifdef CALC_TESTS_DEV_ENABLED
		microseconds
#else
		milliseconds
#endif
		>(end - start));


#ifdef CALC_TEST_FASTFLOAT
	output += "\r\n\r\n" + test_fast_float_parsing();
#endif
	return output;
}

#endif
