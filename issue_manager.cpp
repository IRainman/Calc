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
void IssueManager::report_error(const EquationSize pos,
                                const char *text) noexcept {
  _errors.emplace_back(pos, text);
}

/**
 * Indicate whether any messages have been reported so far.
 */
[[nodiscard]] bool IssueManager::has_errors() noexcept {
  return !_errors.empty();
}

/**
 * Clear the manager.
 */
void IssueManager::clear() noexcept { _errors.clear(); }
#endif
