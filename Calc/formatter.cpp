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
	//return fmt::format(FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::digits10 - 7);      // ok: 313, failed: 9
	//return fmt::format(FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::digits10 - 6);      // ok: 313, failed: 9
	//return fmt::format(FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::digits10 - 5);      // ok: 313, failed: 9
	//return fmt::format(FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::digits10 - 4);      // ok: 313, failed: 9
	//return fmt::format(FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::digits10 - 3);  	// ok: 313, failed: 9
	//return fmt::format(FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::digits10 - 2);  	// ok: 312, failed: 10
	//return fmt::format(FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::digits10 - 1);  	// ok: 311, failed: 11
	//return fmt::format(FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::digits10);			// ok: 307, failed: 15
	//return fmt::format(FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::max_digits10);		// ok: 242, failed: 80
	return fmt::format(FMT_COMPILE("{}"), value);														// ok: 242, failed: 80

	/*
	std::string res = fmt::format(FMT_COMPILE("{}"), value);
	uint_fast16_t max_digits = std::numeric_limits<Value>::max_digits10;
	const auto inf_nan = res == "nan" || res == "inf";
	if (!inf_nan)
	{
		const auto s = res.size();
		const auto pt = res.find_first_of('.');
		const auto e = res.rfind('e');
		return res;
	}
	return res;
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