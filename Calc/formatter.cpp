// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "pch.h"
#include "formatter.h"

[[nodiscard]] std::string Formatter::format(Formatter::Value value) noexcept
{
	//return fmt::format(FMT_COMPILE("{:.{}g}"), value, std::numeric_limits<Value>::digits10);
	return fmt::format(FMT_COMPILE("{}"), value);
}


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