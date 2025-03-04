// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "pch.h"
#include "formatter.h"

[[nodiscard]] std::string Formatter::format(Formatter::Value value) noexcept
{
	//return std::format("{:.{}g}", value, std::numeric_limits<Value>::digits10/* - 3*/);
	return std::format("{}", value);
}

[[nodiscard]] std::string Formatter::create_summary() noexcept
{
	std::string ret;
	for (const auto& error : IssueManager::_errors)
	{
		ret += std::format("Error at pos {}: ", error.pos) + error.text + "\r\n";
	}
	return ret;
}