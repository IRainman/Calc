/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "identifiers.hpp"

enum class Issue : ParamCount {
  unparsable = 0,
  extraneous_input = 1,
  too_many_in_expression,
  expected_left_parenthesis,
  expected_right_parenthesis,
  incorrect_parameters_count,
  too_many_parameters,
  expected_number,
  expected_valid_expression,
  _count
};
constexpr static const auto issue_text = std::to_array<std::string_view>(
    {"unparsable", "extraneous input", "too many ^ in expression",
     "expected left parenthesis", "expected right parenthesis",
     "incorrect parameters count", "too many parameters", "expected number",
     "expected valid expression"});
static_assert(static_cast<size_t>(Issue::_count) == issue_text.size());

struct Token {
  enum class Type : ParamCount {
    // https://en.cppreference.com/w/cpp/language/ascii
    RESULT = 0x00,

    ADD = '+',
    SUB = '-',

    MUL = '*',
    DIV = '/',

    POW = '^',

    LPAREN = '(',
    RPAREN = ')',

    COMA = ',',

    NUM = '0',

    FUNCT = 'A',

    CONST = '_',

    ISSUE = 0x7F,
  };

  union {
    // If token is a number this is the value of it.
    [[no_unique_address]] Value number [[indeterminate]];

    // If token is a identifier or a constant, there is a pointer to it.
    [[no_unique_address]] Identifiers::map::const_pointer identifier
        [[indeterminate]];

    // If token is an error, there is a index of message for it.
    [[no_unique_address]] Issue index [[indeterminate]];
  };

  // Position of this token in the input string.
  [[no_unique_address]] EquationPosition position [[indeterminate]];

  // Type of this token.
  [[no_unique_address]] Type type [[indeterminate]];

  constexpr inline Token(const Type t) noexcept : type(t) {}

  constexpr inline Token() noexcept { /*no init here!*/ };
};

/**
 * Report a new issue.
 */
inline void issue(Token &current, const Issue index) noexcept {
  current.type = Token::Type::ISSUE;
  current.index = index;
}

#endif
