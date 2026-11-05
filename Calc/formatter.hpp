/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef FORMATTER_HPP
#define FORMATTER_HPP

#include "token.hpp"

namespace Calc {

char *format(const Value value, Result &ret) noexcept;

char *format(const Token &token, Result &ret);

} // namespace Calc

#endif
