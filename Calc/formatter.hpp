#pragma once

/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

class Formatter {
public:
  using Result = std::array<char, std::hardware_destructive_interference_size>;
  /**
   * Format output value of the expression returned. Return the end of formated
   * value.
   */
  static char *format(Value value, Result &ret) noexcept;
#ifndef CALC_USE_ERROR_TOKEN
  using Summary = std::array<char, std::hardware_destructive_interference_size>;
  /**
   * Create full report of expression processing. Return the end of formated
   * summary.
   */
  static char *create_summary(Summary &ret) noexcept;
#endif
};
