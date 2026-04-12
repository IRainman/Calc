// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#include "pch.hpp"
#ifndef CALC_USE_ERROR_TOKEN
#include "issue_manager.hpp"

IssueManager::Issues IssueManager::_errors;
/**
 * Report a new error.
 */
void IssueManager::report_error(const EquationSize pos, std::string_view&& text) noexcept
{
	_errors.emplace_back(Issue(std::move(text), pos));
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
}
#endif