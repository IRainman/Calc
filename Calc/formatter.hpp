#pragma once

/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

class Formatter {
public:
  /**
   * Stack type for result formatting
   */
  using Result = std::array<char, zmij::double_buffer_size>;
  /**
   * Format value. Return the end of the formated text.
   */
  static char *format(Value value, Result &ret) noexcept;
#ifndef CALC_USE_ERROR_TOKEN
  /**
   * Stack type for summary of issues formatting
   */
  using Summary = std::array<char,
#ifdef CALC_TESTS_DEV_ENABLED
	  1024
#else
      std::hardware_destructive_interference_size
#endif
  >;
  /**
   * Create report from IssueManager. Return the end of formated text.
   */
  static char *create_summary(Summary &ret) noexcept;
#endif
};
