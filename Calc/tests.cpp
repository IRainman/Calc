// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2023-2025 Solomina Elle, a.rainman on gmail point com
 */

#include "pch.h"
#ifdef CALC_TESTS_ENABLED
#include <chrono>
#include "lexer.h"
#include "parser.h"
#include "issue_manager.h"
#include "formatter.h"
#include "tests.h"

constexpr auto tests = std::to_array<std::pair<std::string_view, Value>>
({
	// syntax errors
	{ "2 + )", std::numeric_limits<Value>::quiet_NaN() },
	{ "2 + (", std::numeric_limits<Value>::quiet_NaN() },
	{ "e(", std::numeric_limits<Value>::quiet_NaN() },
	{ "pi(e)", std::numeric_limits<Value>::quiet_NaN() },
	{ "pi(sin)", std::numeric_limits<Value>::quiet_NaN() },
	{ "sin + 12", std::numeric_limits<Value>::quiet_NaN() },
	{ "sin(1, 2, 3)", std::numeric_limits<Value>::quiet_NaN() },
	{ "1(", std::numeric_limits<Value>::quiet_NaN() },
	{ "sin(0 ", std::numeric_limits<Value>::quiet_NaN() },
	{ "sin(", std::numeric_limits<Value>::quiet_NaN() },
	{ "1+1 1", std::numeric_limits<Value>::quiet_NaN() },
	{ "1+1;", std::numeric_limits<Value>::quiet_NaN() },
	{ "+1.4e-3", std::numeric_limits<Value>::quiet_NaN() },

	// very big or very small numbers: it's should return NAN, we don't need to round it to 0 or inf.
	{ "9999999999999999999e1000000000000000000000000000000", std::numeric_limits<Value>::quiet_NaN() },
	{ "999999999999999999e-1000000000000000000000000000000", std::numeric_limits<Value>::quiet_NaN() },

	// check constants
	// https://en.cppreference.com/w/cpp/numeric/constants
	{ "phi", 1.6180339887498948482045868343656381177203091798057628621 }, // https://en.wikipedia.org/wiki/Golden_ratio
	{ "pi", 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093844609550582231725359408128481117450284102701938521105559644622948954930381964428810975665933446128475648233786783165271201909145648566923460348610454326648213393607260249141273724587006606315588174881520920962829254091715364367892590360011330530548820466521384146951941511609433057270365759591953092186117381932611793105118548074462379962749567351885752724891227938183011949129833673362440656643086021394946395224737190702179860943702770539217176293176752384674818467669405132000568127145263560827785771342757789609173637178721468440901224953430146549585371050792279689258923542019956112129021960864034418159813629774771309960518707211349999998372978 }, // https://en.wikipedia.org/wiki/Pi_(mathematical_constant)
	{ "e", 2.718281828459045235360287471352 }, // https://en.wikipedia.org/wiki/E_(mathematical_constant)
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
	{ "m_n", 1.67492750056858585858585858585858585858585858585858585858585858585e-27 }, // Neutron mass (kg)
	{ "m_u", 1.66053906892e-27 }, // Atomic mass constant (kg)
	


	// check additional constants
	// Gelfond's constant https://en.wikipedia.org/wiki/Gelfond%27s_constant
	{ "e ^ pi", 23.14069263277926900572 },
	{ "pi ^ e", 22.45915771836104547342 },

	// check long mantissa to produce correct output without noise
	{ "0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000001234567890123456789012345",
	0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000001234567890123456789012345 },
	{ "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890.123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890",
	123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890.123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890 },

	// scientific notation
	{ "1.4e-3", 0.0014 },
	{ "-1.4e+3", -1400.0 },
	{ "-92666518056446206563E3", -9.2666518056446206563e+22 },
	{ "1e-3", 0.001 },
	{ "1e+0", 1.0 },
	{ "1e+1", 10.0 },
	{ "1e+2", 100.0 },
	{ "1e3", 1000.0 },
	{ "1e+4", 10000.0 },
	{ "1e5", 100000.0 },
	{ "1e+6", 1000000.0 },
	{ "1e+7", 10000000.0 },

	// basic operations
	{ "1+2", 3.0 },
	{ "5-3", 2.0 },
	{ "4*2", 8.0 },
	{ "4*4*4*4*4*4*3*3*3*3", 331776.0 },
	{ "8/4", 2.0 },
	{ "2^3", 8.0 },
	{ "5%2", 1.0 },

	// advanced functions
	{ "exp(0)", 1.0 },
	{ "log1p(1e-16)", 1.0e-16 },
	{ "expm1(1e-16)", 1.0e-16 },
	{ "tgamma(5)", 24.0 },  // 4! = 24
	//{ "lgamma(5)", std::log(24) },
	{ "beta(2, 2)", 0.166666666666666666666666666666666666666666666 },
	{ "erf(0)", 0.0 },
	{ "erfc(0)", 1.0 },
	{ "mod(5, 2)", 1.0 },

	// logarithmic functions
	{ "ln(e)",1.0 },
	{ "log10(1000)",3.0 },
	{ "log2(8)",3.0 },
	{ "log(sh(42) + ch(42))", 42 },
	{ "log2(2^3)", 3.0 },
	{ "log10(10^3)", 3.0 },

	// hyperbolic functions https://en.wikipedia.org/wiki/Hyperbolic_functions
	{ "sh(0)", 0.0 },
	{ "ch(0)", 1.0 },
	{ "tanh(0)", 0.0 },
	{ "asinh(0)", 0.0 },
	{ "acosh(1)", 0.0 },
	{ "atanh(0)", 0.0 },

	// misc functions
	{ "abs(-42)", 42.0 },
	{ "ceil(pi)", 4.0 },
	{ "floor(pi)", 3.0 },
	{ "round(pi)", 3.0 },
	{ "trunc(pi)", 3.0 },
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
	{ "0.1 + 0.1", 0.2 },
	{ "0.1 + 0.1 + 0.1", 0.3 },
	{ "0.1 + 0.1 + 0.1 + 0.1", 0.4 },
	{ "0.1 + 0.1 + 0.1 + 0.1 + 0.1", 0.5 },
	{ "0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1", 0.6 },
	{ "0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1", 0.7 },
	{ "0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1", 0.8 },
	{ "0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1", 0.9 },
	{ "0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1", 1.0 },
	{ "100.0 - 99.99", 0.01 },
	{ "10.0 - 9.99", 0.01 },
	{ "1.0 - 0.99", 0.01 },
	{ "0.1 - 0.09", 0.01 },
	{ "0.01 - 0.009", 0.001 },
	{ "0.001 - 0.0009", 0.0001 },
	{ "0.0001 - 0.00009", 0.00001 },
	{ "0.00001 - 0.000009", 0.000001 },
	{ "0.000001 - 0.0000009", 0.0000001 },
	{ "0.0000001 - 0.00000009", 0.00000001 },
	{ "0.00000001 - 0.000000009", 0.000000001 },
	{ "0.000000001 - 0.0000000009", 0.0000000001 },
	{ "0.0000000001 - 0.00000000009", 0.00000000001 },
	{ "0.00000000001 - 0.000000000009", 0.000000000001 },
	{ "0.000000000001 - 0.0000000000009", 0.0000000000001 },
	{ "0.0000000000001 - 0.00000000000009", 0.00000000000001 },
	{ "0.00000000000001 - 0.000000000000009", 0.000000000000001 },
	{ "0.000000000000001 - 0.0000000000000009", 0.0000000000000001 },
	{ "0.0000000000000001 - 0.00000000000000009", 0.00000000000000001 },
	{ "0.2 + 0.1", 0.3 },
	{ "0.1 + 0.2", 0.3 },
	{ "10.0 - 0.93", 9.07 },
	{ "10.0 - 0.94", 9.06 },
	{ "10.0 - 0.95", 9.05 },
	{ "10.0 - 0.96", 9.04 },
	{ "10.0 - 0.97", 9.03 },
	{ "10.0 - 0.98", 9.02 },
	{ "10.0 - 0.99", 9.01 },
	{ "10.0 - 1.0",  9.0 },
	{ "10.0 - 1.01", 8.99 },
	{ "10.0 - 1.02", 8.98 },
	{ "10.0 - 1.03", 8.97 },
	{ "10.0 - 1.04", 8.96 },
	{ "10.0 - 1.05", 8.95 },
	{ "10.0 - 1.06", 8.94 },
	{ "10.0 - 1.07", 8.93 },
	{ "10.0 - 1.08", 8.92 },
	{ "10.0 - 1.09", 8.91 },
	{ "10.0 - 1.1",  8.9 },
	{ "10.0 - 1.11", 8.89 },
	{ "10.0 - 1.12", 8.88 },
	{ "10.0 - 1.13", 8.87 },
	{ "10.0 - 1.14", 8.86 },
	{ "10.0 - 1.15", 8.85 },
	{ "10.0 - 1.16", 8.84 },
	{ "10.0 - 1.17", 8.83 },
	{ "10.0 - 1.18", 8.82 },
	{ "10.0 - 1.19", 8.81 },
	{ "10.0 - 1.2",  8.8 },

	// trigonometric precision especially with pi
	{ "sin(pi/2)", 1.0 },
	{ "cos(pi/2)", 0.0 },
	{ "sin(pi)", 0.0 },
	{ "cos(pi)", -1.0 },
	{ "sin(3*pi/2)", -1.0 },
	{ "cos(3*pi/2)", 0.0 },
	{ "tan(3*pi/2)", std::numeric_limits<Value>::infinity() },
	{ "87 * tan(pi) - 7", -7.0 },
	{ "tan(-pi)", 0.0 },
	{ "tan(pi)", 0.0 },
	{ "tan(pi/2)", std::numeric_limits<Value>::infinity() },
	{ "tan(-pi/2)", -std::numeric_limits<Value>::infinity() },
	{ "tan(pi/4)", 1.0 },
	{ "tan(-pi/4)", -1.0 },
	{ "tan(3*pi/4)", -1.0 },
	{ "tan(-3*pi/4)", 1.0 },
	{ "tan(5*pi/4)", 1.0 },
	{ "tan(-5*pi/4)", -1.0 },
	{ "tan(7*pi/4)", -1.0 },
	{ "tan(-7*pi/4)", 1.0 },
	{ "tan(9*pi/4)", 1.0 },
	{ "tan(-9*pi/4)", -1.0 },
	{ "tan(11*pi/4)", -1.0 },
	{ "tan(-11*pi/4)", 1.0 },
	{ "tan(13*pi/4)", 1.0 },
	{ "tan(-13*pi/4)", -1.0 },
	{ "tan(15*pi/4)", -1.0 },
	{ "tan(-15*pi/4)", 1.0 },
	{ "tan(17*pi/4)", 1.0 },
	{ "tan(-17*pi/4)", -1.0 },
	{ "tan(19*pi/4)", -1.0 },
	{ "tan(-19*pi/4)", 1.0 },
	{ "tan(21*pi/4)", 1.0 },
	{ "tan(-21*pi/4)", -1.0 },
	{ "tan(23*pi/4)", -1.0 },
	{ "tan(-23*pi/4)", 1.0 },
	{ "tan(25*pi/4)", 1.0 },
	{ "tan(-25*pi/4)", -1.0 },
	{ "tan(27*pi/4)", -1.0 },
	{ "tan(-27*pi/4)", 1.0 },

	// special value support
	{ "inf", std::numeric_limits<Value>::infinity() },
	{ "1 / 0", std::numeric_limits<Value>::infinity() },
	{ "0 / 0", std::numeric_limits<Value>::quiet_NaN() },
	{ "sqrt(4 - 9)", std::numeric_limits<Value>::quiet_NaN() },

	// trigonometric functions
	{ "rad(180)", 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093844609550582231725359408128481117450284102701938521105559644622948954930381964428810975665933446128475648233786783165271201909145648566923460348610454326648213393607260249141273724587006606315588174881520920962829254091715364367892590360011330530548820466521384146951941511609433057270365759591953092186117381932611793105118548074462379962749567351885752724891227938183011949129833673362440656643086021394946395224737190702179860943702770539217176293176752384674818467669405132000568127145263560827785771342757789609173637178721468440901224953430146549585371050792279689258923542019956112129021960864034418159813629774771309960518707211349999998372978 }, // https://en.wikipedia.org/wiki/Pi_(mathematical_constant)
	{ "deg(pi)", 180.0 },
	{ "sin(0)", 0.0 },
	{ "cos(0)", 1.0 },
	{ "tan(0)", 0.0 },
	{ "cos(0)", 1.0 },
	{ "tan(0)", 0.0 },
	{ "sin(rad(30))", 0.5 },
	{ "cos(rad(60))", 0.5 },
	{ "tan(rad(45))", 1.0 },
	{ "sin(pi/4)^2", 0.5 },
	{ "cos(pi/3)^2", 0.25 },
	{ "sin(0)^2 + cos(0)^2", 1.0 },
	{ "sin(rad(30)) + cos(rad(60))", 1.0 },
	{ "sin(rad(30)) - cos(rad(60))", 0.0 },
	{ "sin(rad(30)) * cos(rad(60))", 0.25 },
	{ "sin(rad(30)) / cos(rad(60))", 1.0 },
	{ "cos(rad(30)) / sin(rad(60))", 1.0 },
	{ "arcsin(1)", 1.57079632679489661923132169163975144 },
	{ "arccos(1)", 0.0 },
	{ "arctan(1)", 0.78539816339744830961566084581987572 },

	// power and root functions
	{ "pow(2,3)", 8.0 },
	{ "sqrt(4)", 2.0 },
	{ "sqrt(4) + sqrt(9)", 5.0 },
	{ "sqrt(4) - sqrt(9)", -1.0 },
	{ "sqrt(4) * sqrt(9)", 6.0 },
	{ "sqrt(4) / sqrt(9)", 0.6666666666666666 },
	{ "sqrt(4) ^ sqrt(9)", 8.0 },
	{ "sqrt(4 + 9)", 3.605551275463989 },
	{ "sqrt(4 * 9)", 6.0 },
	{ "sqrt(4 / 9)", 0.6666666666666666 },
	{ "sqrt(4 ^ 9)", 512.0 },
	{ "sqrt(4 + 9) ^ 2", 13.0 },
	{ "sqrt(4 * 9) ^ 2", 36.0 },
	{ "sqrt(4 / 9) ^ 2", 0.4444444444444444 },
	{ "sqrt(4 ^ 9) ^ 2", 262144.0 },
	{ "cbrt(27)", 3.0 },
	{ "cbrt(27) + cbrt(64)", 7.0 },
	{ "cbrt(27) - cbrt(64)", -1.0 },
	{ "cbrt(27) * cbrt(64)", 12.0 },
	{ "cbrt(27) / cbrt(64)", 0.75 },
	{ "cbrt(27) ^ cbrt(64)", 81.0 },
	{ "cbrt(27 + 64)", 4.497941445275415 },
	{ "cbrt(27 * 64)", 12.0 },
	{ "cbrt(27 / 64)", 0.75 },
	{ "cbrt(27 ^ 64)", 3.433683820292513e+30 },
	{ "0^0", 1.0 },
	{ "-4 ^ 2", 16.0 },
	{ "0 - 4 ^ 2", -16.0 },
	{ "3 ^ 3 ^ 3", 7625597484987.0 },
	{ "(3 ^ 3) ^ 3", 19683.0 },
	{ "2 ^ 2 ^ 2 ^ 2", 65536.0 },
	{ "2 ^ 2 ^ 2", 16.0 },
	{ "2 ^ (3 ^ 2)", 512.0 },
	{ "2 ^ (4 ^ 2)", 65536.0 },
	{ "(2 ^ 3) ^ 2", 64.0 },
	{ "(2 ^ 4) ^ 2", 256.0 },

	// min max functions
	{ "min(1)", 1.0 },
	{ "min(1, 2, 3)", 1.0 },
	{ "max(1, 2, 3)", 3.0 },
	{ "min(1, 2, 3, 4)", 1.0 },
	{ "max(1, 2, 3, 4)", 4.0 },
	{ "min(1, 2, 3, 4, 5)", 1.0 },
	{ "max(1, 2, 3, 4, 5)", 5.0 },
	{ "min(1, 2, 3, 4, 5, 6)", 1.0 },
	{ "max(1, 2, 3, 4, 5, 6)", 6.0 },
	{ "min(1, 2, 3, 4, 5, 6, 7)", 1.0 },
	{ "max(1, 2, 3, 4, 5, 6, 7)", 7.0 },
	{ "min(1, 2, 3, 4, 5, 6, 7,8)", 1.0 },
	{ "max(1,2 ,3 ,4 ,5 ,6 ,7 ,8)", 8.0 },
	{ "min(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14)", 1.0 },
	{ "max(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14)", 14.0 },

	// hardware and library present
	{ "fma(0.2, 0.1, 0.3)", 0.32 },

	// additional special
	{ "distance(1, 1, 2, 3)", 6.0 }
	});

#ifdef CALC_TEST_FASTFLOAT
[[nodiscard]] inline Value bin(const std::string_view& x) noexcept
{
	uint32_t bin_val;
	auto res = fast_float::from_chars(x.data(), x.data() + x.size(), bin_val, 2);
	if (res.ec == std::errc{}) [[likely]]
	{
		return bin_val;
	}

	[[unlikely]]
	return std::numeric_limits<Value>::quiet_NaN();
}
[[nodiscard]] inline Value hex(const std::string_view& x) noexcept
{
	uint32_t hex_val;
	auto res = fast_float::from_chars(x.data(), x.data() + x.size(), hex_val, 16);
	if (res.ec == std::errc{}) [[likely]]
	{
		return hex_val;
	}

	[[unlikely]]
	return std::numeric_limits<Value>::quiet_NaN();
}

static std::string test_fast_float_parsing()
{
	uint32_t bin_val, hex_val;
	float f_val;
	constexpr std::string_view data_bin = "1010010010000011001001010101001";
	auto res = fast_float::from_chars(data_bin.data(), data_bin.data() + data_bin.size(), bin_val, 2);
	constexpr std::string_view data_hex = "abCdEf69";
	res = fast_float::from_chars(data_hex.data(), data_hex.data() + data_hex.size(), hex_val, 16);
	constexpr std::string_view data_float = "12345678e-9";
	res = fast_float::from_chars(data_float.data(), data_float.data() + data_float.size(), f_val);
	return fmt::format(FMT_COMPILE("bin:{}: {},\r\nhex:{}: {},\r\nfloat:{}: {}"), data_bin, bin_val, data_hex, hex_val, data_float, f_val);
}
#endif

std::string calc_tests()
{
	std::string output;
#ifdef CALC_TESTS_DEV_ENABLED
	output.reserve(60000);
	size_t failed = 0;
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

			[[maybe_unused]] const auto value = p.parse();
			const bool has_errors = IssueManager::has_errors();
#ifdef CALC_TESTS_DEV_ENABLED
			const bool ok = (has_errors && std::isnan(t.second)) || (std::isnan(value) && std::isnan(t.second)) || (Formatter::format(value) == Formatter::format(t.second));

			if(!ok)
			{
				++failed;
			}
			output += fmt::format(FMT_COMPILE(
				"Test {}: {}\r\n"
				"{}\r\n"
				"{}\r\n"),
				ok ? "OK" : "FAILED",
				t.first,
					fmt::format(FMT_COMPILE(
					"return = {}\r\n"
					"expect = {}\r\n"
					"output = {}"),
					value,
					t.second,
					Formatter::format(value)
					),
				Formatter::create_summary());
#endif
			if (has_errors)
			{
				IssueManager::clear();
			}
		}
	}

	const auto end = std::chrono::steady_clock::now();

	// Don't use fmt here, because it isn't compile.
	output += std::format("Tests"
#ifdef CALC_TESTS_DEV_ENABLED
		":\r\n ok: {},\r\n failed: {}\r\n"
#endif
		" time is: {}.",
#ifdef CALC_TESTS_DEV_ENABLED
		tests.size() - failed, failed,
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
