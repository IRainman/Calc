/*
 * Copyright 2023 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "stdafx.h"
#include "formatter.h"

static Formatter f;

Formatter& Formatter::get_instance() noexcept
{
	return f;
}

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

std::string Formatter::format_severity(const Issue::Severity severity)
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

std::string Formatter::format_issue(const Issue& issue)
{
	return format_severity(issue.severity) + " at pos " + std::to_string(issue.pos) + ": " + issue.text;
}
