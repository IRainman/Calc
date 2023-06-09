// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

//
// Prototype writed by Tamika Nomara on 28.05.2023.
//
// Full version implemented by Elle Solomina and maintained since 29.05.2023.
//

#include "pch.h"
#include "issue_manager.h"

static IssueManager im;

IssueManager& IssueManager::get_instance() noexcept
{
	return im;
}

void IssueManager::report(Issue&& issue)
{
	_has_errors |= _messages.emplace_back(std::move(issue)).severity == Issue::Severity::ERR;
}

void IssueManager::report(Issue::Severity severity, size_t pos, std::string&& text)
{
	report(Issue{std::move(text), severity, pos});
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
