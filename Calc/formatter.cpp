// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2023-2025 Solomina Elle, a.rainman on gmail point com
 */

#include "pch.h"
#include "formatter.h"
#include "issue_manager.h"

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

#if !defined(CALC_USE_ZMIJ) || defined (CALC_TESTS_DEV_ENABLED) && !defined(CALC_USE_ERROR_TOKEN)
[[nodiscard]] std::string Formatter::format(Value value) noexcept
{
	// Various experiments with formatting options:
	// TODO add to GUI?: constexpr uint8_t kValueDigits10[3] = { (std::numeric_limits<Value>::digits10 - (std::numeric_limits<Value>::max_digits10 - std::numeric_limits<Value>::digits10) - 1), std::numeric_limits<Value>::digits10, std::numeric_limits<Value>::max_digits10 };
	//return fmt::format(FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::digits10 - 3 /*12*/);	// exactly: 310, less than epsilon: 19, failed: 9
	//return fmt::format(FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::digits10 - 2);			// exactly: 309, less than epsilon: 19, failed: 10
	//return fmt::format(FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::digits10 - 1);			// exactly: 308, less than epsilon: 19, failed: 11
	//return fmt::format(FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::digits10     /*15*/);	// exactly: 304, less than epsilon: 19, failed: 15
	//return fmt::format(FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::max_digits10 /*17*/);	// exactly: 235, less than epsilon: 19, failed: 84
	return fmt::to_string(value);//return fmt::format(FMT_COMPILE("{}"), value /*17*/);						// exactly: 235, less than epsilon: 19, failed: 84
}
#endif

#ifndef CALC_USE_ERROR_TOKEN
[[nodiscard]] std::string Formatter::create_summary() noexcept
{
	// https://fmt.dev/11.1/api/#range-and-tuple-formatting
	std::string ret;
	//ret.resize(1024);
	//auto begin = ret.data();
	for (const auto& error : IssueManager::_errors)
	{
		ret += fmt::format(FMT_COMPILE("Error at pos {}: {}\r\n"), error.pos, error.text);
		//begin = fmt::format_to(begin, FMT_COMPILE("Error at pos {}: {}\r\n"), error.pos, error.text);
	}
	return ret;
}
#endif