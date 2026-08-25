/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef ISSUE_MANAGER_HPP
#define ISSUE_MANAGER_HPP

#ifndef CALC_USE_ERROR_TOKEN
#include <vector>

/**
 * Represents a message from the compiler.
 */
class Issue {
public:
  Issue(const EquationSize _pos, const char *_text) noexcept
      : pos(_pos), text(_text) {};
  Issue(const Issue &) = delete;
  Issue(Issue &&) = default;

private:
  // Position within the context at which the issue has occurred.
  const EquationSize pos;

  // Issue describing of the message.
  const char *text;

  friend class Formatter;
};

/**
 * Reports and prints issues.
 */
class IssueManager {
  using Issues = std::vector<Issue>;

public:
  /**
   * Report a new error.
   */
  static void report_error(const EquationSize pos, const char *text) noexcept;

  /**
   * Indicate whether any messages have been reported so far.
   */
  [[nodiscard]] static bool has_errors() noexcept;

  /**
   * Clear the manager.
   */
  static void clear() noexcept;

private:
  static Issues _errors; // Errors that do stop processing.
  friend class Formatter;
};

#endif
#endif
