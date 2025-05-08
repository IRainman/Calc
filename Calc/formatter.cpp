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
	//return fmt::format(FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::digits10/* - 3*/);
	return fmt::format(FMT_COMPILE("{}"), value);
}

#ifndef CALC_USE_ERROR_TOKEN
[[nodiscard]] std::string Formatter::create_summary() noexcept
{
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