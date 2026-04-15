// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#include "pch.hpp"
#include "formatter.hpp"
#include "issue_manager.hpp"

#ifdef CALC_USE_ZMIJ
#include "../../zmij/zmij.cc"
#if CALC_USE_128_BIT_FLOAT
#warning "128-bit float type isn't supported by zmij. The library convert any user input values to 64-bit double."
#endif
#endif

#ifdef CALC_SUPPORT_FRACTIONAL_OUTPUT
// Recursive function to
// return GCD of a and b
int64_t gcd(int64_t a, int64_t b)
{
	if (a == 0)
		return b;
	else if (b == 0)
		return a;
	if (a < b)
		return gcd(a, b % a);
	else
		return gcd(b, a % b);
}

// Function to convert decimal to fraction
std::pair<int64_t, int64_t> decimalToFraction(Value number)
{
	if (std::isnan(number) || std::isinf(number))
		return { 0,1 };

	// Fetch integral value of the decimal
	Value intVal = std::floor(number);

	// Fetch fractional part of the decimal
	Value fVal = number - intVal;

	// Consider precision value to
	// convert fractional part to
	// integral equivalent
	constexpr int64_t pVal = 1e15;

	// Calculate GCD of integral
	// equivalent of fractional
	// part and precision value
	int64_t gcdVal = gcd(std::round(fVal * pVal), pVal);

	// Calculate num and deno
	int64_t num = std::round(fVal * pVal) / gcdVal;
	int64_t deno = pVal / gcdVal;

	if (std::abs(num) > 1e14 || deno == 1 || deno > 1e14)
		return { 0,1 };

	// Return the fraction
	return std::make_pair((intVal * deno) + num, deno);
}
#endif

char* Formatter::format(Value value, Result& ret) noexcept
{
	// Various experiments with formatting options:
	// https://www.exploringbinary.com/decimal-precision-of-binary-floating-point-numbers/
#ifdef CALC_USE_ZMIJ
	return zmij::detail::write(value, ret.data());
#else
	// TODO add to GUI?: constexpr uint8_t kValueDigits10[3] = { (std::numeric_limits<Value>::digits10 - (std::numeric_limits<Value>::max_digits10 - std::numeric_limits<Value>::digits10) - 1), std::numeric_limits<Value>::digits10, std::numeric_limits<Value>::max_digits10 };
	//return fmt::format_to(ret.data(), FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::digits10 - 7	/*08*/);	// exactly: 271, less than epsilon: 77, failed: 12
	//return fmt::format_to(ret.data(), FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::digits10 - 6	/*09*/);	// exactly: 271, less than epsilon: 77, failed: 12
	//return fmt::format_to(ret.data(), FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::digits10 - 5	/*10*/);	// exactly: 272, less than epsilon: 77, failed: 11
	//return fmt::format_to(ret.data(), FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::digits10 - 4	/*11*/);	// exactly: 272, less than epsilon: 77, failed: 11
	//return fmt::format_to(ret.data(), FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::digits10 - 3	/*12*/);	// exactly: 271, less than epsilon: 77, failed: 12
	//return fmt::format_to(ret.data(), FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::digits10 - 2	/*13*/);	// exactly: 270, less than epsilon: 77, failed: 13
	//return fmt::format_to(ret.data(), FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::digits10 - 1	/*14*/);	// exactly: 269, less than epsilon: 77, failed: 14
	return fmt::format_to(ret.data(), FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::digits10		/*15*/);	// exactly: 264, less than epsilon: 77, failed: 19
	//return fmt::format_to(ret.data(), FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::digits10 + 1	/*16*/);	// exactly: 214, less than epsilon: 78, failed: 69
	//return fmt::format_to(ret.data(), FMT_COMPILE("{}"), value											/*15...17*/);	// exactly: 189, less than epsilon: 78, failed: 94
	//return fmt::format_to(ret.data(), FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::max_digits10	/*17*/);	// exactly: 214, less than epsilon: 78, failed: 69
#endif
}

#ifndef CALC_USE_ERROR_TOKEN
char* Formatter::create_summary(Summary& ret) noexcept
{
	auto begin = ret.data();
	for (const auto& error : IssueManager::_errors)
	{
		begin = fmt::format_to(begin, FMT_COMPILE("{}: {}\r\n"), error.pos, error.text);
	}
	return begin;
}
#endif