// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

//
// Created by Tamika Nomara on 28.05.2023.
//
//
// Manteined by Elle Solomina since 29.05.2023.
//

#include "stdafx.h"

#include "issue_manager.h"

static IssueManager im;

IssueManager& IssueManager::get_instance() noexcept
{
	return im;
}

void IssueManager::report(Message&& issue)
{
	_has_errors |= _messages.emplace_back(std::move(issue)).severity == Message::Severity::ERR;
}

void IssueManager::report(Message::Severity severity, size_t pos, std::string&& text)
{
	report(Message{std::move(text), severity, pos});
}

void IssueManager::report_info(size_t pos, std::string&& text)
{
	report(Message::Severity::INFO, pos, std::move(text));
}

void IssueManager::report_warning(size_t pos, std::string&& text)
{
	report(Message::Severity::WARN, pos, std::move(text));
}

void IssueManager::report_error(size_t pos, std::string&& text)
{
	report(Message::Severity::ERR, pos, std::move(text));
}

[[deprecated("needs to move from here to another new class, like formatter, or something like this")]]
[[nodiscard]] const std::string& IssueManager::create_summary_and_clear()
{
	_summary.clear();
	_summary += "Completed with "
	            + std::to_string(_messages.size())
	            + " message"
	            + (_messages.size() == 1 ? "" : "s")
	            + (_messages.empty() ? "." : ":")
	            + "\r\n";
	            
	for (auto& message : _messages)
	{
		_summary += "In position " + std::to_string(message.pos) + ": " + std::move(message.text) + "\r\n";
	}
	clear();
	
	return _summary;
}

#if 0

[[deprecated("needs to move from here to another new class, like formatter, or something like this")]]
std::ostream& operator<<(std::ostream& os, const Issue::Severity severity)
{
	switch (severity)
	{
		case Issue::Severity::INFO:
			return os << "Info";
		case Issue::Severity::WARN:
			return os << "Warning";
		case Issue::Severity::ERR:
			return os << "Error";
	}
	__assume(false); // C++23 unreachable();
}

[[deprecated("needs to move from here to another new class, like formatter, or something like this")]]
std::ostream& operator<<(std::ostream& os, const Issue& issue)
{
	os << issue.severity << " at pos:" << issue.pos << ": " << issue.message.str(); //-V128
	return os;
}

#endif
