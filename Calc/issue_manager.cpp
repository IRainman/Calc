// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "pch.h"
#include <vector>
#include "issue_manager.h"

std::vector<Issue> IssueManager::_messages;
bool IssueManager::_has_errors = false;

void IssueManager::report_info(size_t pos, std::string&& text) noexcept
{
	_messages.emplace_back(Issue(std::move(text), pos, Issue::Severity::INFO));
}

void IssueManager::report_warning(size_t pos, std::string&& text) noexcept
{
	_messages.emplace_back(Issue(std::move(text), pos, Issue::Severity::WARN));
}

void IssueManager::report_error(size_t pos, std::string&& text) noexcept
{
	_messages.emplace_back(Issue(std::move(text), pos, Issue::Severity::ERR));
	_has_errors = true;
}
