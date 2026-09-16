/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#include "pch.hpp"

#include "formatter.hpp"
#include "issue_manager.hpp"

using Issues = std::vector<Token>;

Issues static issues;

/**
 * Report a new issue.
 */
Token &issue(Token &current, const EquationSize position,
             const Issue index) noexcept {
  if (!issues.empty()) {
    auto i = issues.back().issue;
    if (i.index == index && i.position == position) {
      return issues.back();
    }
  }
  current.type = Token::Type::ISSUE;
  current.issue.position = static_cast<uint32_t>(position);
  current.issue.index = index;
  return issues.emplace_back(current);
}

/**
 * @return the end of formated text.
 */
char *report(Result &ret) noexcept {
  auto end = ret.data();
  for (const auto &i : issues) {
    const auto &issue = i.issue;
    end = fmt::format_to(end, FMT_COMPILE("{}: {}\n"), issue.position,
                         issue_text[static_cast<ParamCount>(issue.index)]);
  }
  issues.clear();
  return end;
}
