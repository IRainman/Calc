// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2023-2025 Solomina Elle, a.rainman on gmail point com
 */

#include "pch.h"
#include "issue_manager.h"
#ifndef CALC_USE_ERROR_TOKEN

IssueManager::Issues IssueManager::_errors;
#ifdef ISSUE_MANAGER_HAVE_SEVERITY
IssueManager::Issues IssueManager::_warnings;
IssueManager::Issues IssueManager::_infos;
#endif
#ifndef CALC_USING_STATIC_VECTOR
/**
 * Speedup the manager if needed.
 */
void IssueManager::speedup() noexcept
{
	_errors.reserve(CALC_MAX_ISSUES);
#ifdef ISSUE_MANAGER_HAVE_SEVERITY
	_warnings.reserve(CALC_MAX_ISSUES);
	_infos.reserve(CALC_MAX_ISSUES);
#endif
}
#endif
#ifdef ISSUE_MANAGER_HAVE_SEVERITY
/**
 * Report a new info message.
 */
void report_info(EquationSize pos, std::string&& text) noexcept
{
	emplace(_infos, pos, std::move(text));
}

/**
 * Report a new warning.
 */
void report_warning(EquationSize pos, std::string&& text) noexcept
{
	emplace(_warnings, pos, std::move(text));
}
#endif
/**
 * Report a new error.
 */
void IssueManager::report_error(const EquationSize pos, std::string&& text) noexcept
{
	emplace(_errors, pos, std::move(text));
}

/**
 * Indicate whether any messages have been reported so far.
 */
[[nodiscard]] bool IssueManager::has_errors() noexcept
{
	return !_errors.empty();
}

/**
 * Clear the manager.
 */
void IssueManager::clear() noexcept
{
	_errors.clear();
#ifdef ISSUE_MANAGER_HAVE_SEVERITY
	_warnings.clear();
	_infos.clear();
#endif
}
		
void IssueManager::emplace(Issues& v, EquationSize pos, std::string&& text) noexcept
{
	v.emplace_back(std::move(Issue(std::move(text), pos)));
}

#endif