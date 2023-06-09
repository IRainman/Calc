// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2023 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "stdafx.h"
#include "formatter.h"
#include "issue_manager.h"

static Formatter f;

Formatter& Formatter::get_instance() noexcept
{
	return f;
}

namespace
{
	std::string format_severity(const Issue::Severity severity)
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
		__assume(false); // C++23 unreachable();
	}

	std::string format_issue(const Issue & issue)
	{
		return format_severity(issue.severity) + " at pos " + std::to_string(issue.pos) + ": " + issue.text;
	}
};

[[nodiscard]] const std::string& Formatter::create_summary_and_clear_issue_manager()
{
	const auto& messages = IssueManager::get_instance().messages();
	_summary.clear();
	_summary.append("Completed with "
		+ std::to_string(messages.size())
		+ " message"
		+ (messages.size() == 1 ? "" : "s")
		+ (messages.empty() ? "." : ":")
		+ "\r\n");

	for (const auto& message : messages)
	{
		_summary += format_issue(message);
	}
	IssueManager::get_instance().clear();

	return _summary;
}
