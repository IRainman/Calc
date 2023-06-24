// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2023 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "pch.h"
#include "formatter.h"
#include "issue_manager.h"

namespace
{
	[[nodiscard]] std::string format_severity(const Issue::Severity severity)
	{
		switch (severity)
		{
		case Issue::Severity::INFO:
			return "Info";
		case Issue::Severity::WARN:
			return "Warning";
		case Issue::Severity::ERR:
			return "Error";
		}
		std::unreachable();
	}

	[[nodiscard]] std::string format_issue(const Issue& issue)
	{
		return format_severity(issue.severity) + " at pos " + std::to_string(issue.pos) + ": " + issue.text;
	}
};

[[nodiscard]] std::string Formatter::create_summary()
{
	const auto& messages = IssueManager::get_instance().messages();
	std::string ret = "Completed with "
		+ std::to_string(messages.size())
		+ " message"
		+ (messages.size() == 1 ? "" : "s")
		+ (messages.empty() ? "." : ":")
		+ "\r\n";

	for (const auto& message : messages)
	{
		ret += format_issue(message);
	}

	return ret;
}
