// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "pch.h"
#include "formatter.h"
#include "issue_manager.h"
#include <string>
#include <utility>

[[nodiscard]] std::string Formatter::create_summary() noexcept
{
	const auto& messages = IssueManager::messages();
	std::string ret = "Completed with "
		+ std::to_string(messages.size())
		+ " message"
		+ (messages.size() == 1 ? "" : "s")
		+ (messages.empty() ? "." : ":")
		+ "\r\n";

	for (const auto& message : messages)
	{
		ret += format(message);
	}

	return ret;
}
