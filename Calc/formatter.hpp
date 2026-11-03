/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef FORMATTER_HPP
#define FORMATTER_HPP

#include "token.hpp"

namespace Calc {

char *value(const Value &value, Result &ret) noexcept;

char *result(const Token &value, Result &ret) noexcept;

} // namespace Calc

#endif
