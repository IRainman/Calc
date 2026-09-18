/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef FORMATTER_HPP
#define FORMATTER_HPP

#include "token.hpp"

char *value(const Value value, Result &ret) noexcept;

char *result(const Token &value, Result &ret) noexcept;

#endif
