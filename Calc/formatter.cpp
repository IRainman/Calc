// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2023-2025 Solomina Elle, a.rainman on gmail point com
 */

#include "pch.h"
#include "formatter.h"
#include "issue_manager.h"

[[nodiscard]] std::string Formatter::format(Value value) noexcept
{
	//return fmt::format(FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::digits10 - 2 - 1);	// ok: 285, failed: 9
	//return fmt::format(FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::digits10);			// ok: 280, failed: 14
	//return fmt::format(FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::max_digits10);		// ok: 218, failed: 76
	return fmt::format(FMT_COMPILE("{}"), value);														// ok: 218, failed: 76

	/*
	std::string s = fmt::format(FMT_COMPILE("{}"), value)
	uint_fast16_t max_digits = std::numeric_limits<Value>::max_digits10;
	uint_fast16_t n = 0;
	if (s.find_first_not_of("0123456789", n) == std::string::npos)
	{
		s.resize(max_digits);
		return s;
	}
	if (s.find_first_of('.') != std::string::npos)
	{
		n = s.find_first_of('.');
		if (n < max_digits)
		{
			s.resize(n + 1 + max_digits - n);
			return s;
		}
	}
	else
	{
		s.resize(max_digits);
		return s;
	}
	*/
}

#ifndef CALC_USE_ERROR_TOKEN
[[nodiscard]] std::string Formatter::create_summary() noexcept
{
	// https://fmt.dev/11.1/api/#range-and-tuple-formatting
	std::string ret;
	for (const auto& error : IssueManager::_errors)
	{
		//fmt::memory_buffer buffer;
		ret += fmt::format(FMT_COMPILE("Error at pos {}: "), error.pos) + error.text + "\r\n";
		//ret += std::string(buffer.data(), buffer.size());
	}
	return ret;
}
#endif