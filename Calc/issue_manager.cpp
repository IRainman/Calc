// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "pch.h"
#include "issue_manager.h"

bool IssueManager::_has_errors = false;
std::vector<Issue> IssueManager::_messages;

void IssueManager::report(Issue&& issue)
{
	_has_errors |= _messages.emplace_back(std::move(issue)).severity == Issue::Severity::ERR;
}

void IssueManager::report(Issue::Severity severity, size_t pos, std::string&& text)
{
	report(Issue{std::move(text), pos, severity});
}

void IssueManager::report_info(size_t pos, std::string&& text)
{
	report(Issue::Severity::INFO, pos, std::move(text));
}

void IssueManager::report_warning(size_t pos, std::string&& text)
{
	report(Issue::Severity::WARN, pos, std::move(text));
}

void IssueManager::report_error(size_t pos, std::string&& text)
{
	report(Issue::Severity::ERR, pos, std::move(text));
}
