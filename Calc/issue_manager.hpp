/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef ISSUE_MANAGER_HPP
#define ISSUE_MANAGER_HPP

#include "token.hpp"

Token &issue(Token &current, const EquationSize position,
             const Issue index) noexcept;

char *report(Result &ret) noexcept;

#endif
