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
  expected_right_parenthesis_or_comma,
  incorrect_parameters_count,
  too_many_parameters,
  expected_number,
  _count
};
constexpr static const auto issue_text = std::to_array<std::string_view>(
    {"unparsable", "extraneous input", "too many ^ in expression",
     "expected left parenthesis", "expected right parenthesis",
     "expected right parenthesis or comma", "incorrect parameters count",
     "too many parameters", "expected number"});
static_assert(static_cast<uint8_t>(Issue::_count) == issue_text.size());

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
    // If token is a number parsed from string or a constant, this is the value
    // of it.
    [[no_unique_address]] Value number [[indeterminate]];

    // If token is a identifier or a constant, there is a pointer to it.
    [[no_unique_address]] Identifiers::map::const_pointer identifier
        [[indeterminate]];

    // If token is an error, there is a small helper that handle it.
    struct {
      [[no_unique_address]] EquationSize position [[indeterminate]];
      [[no_unique_address]] Issue index [[indeterminate]];
    } issue;
  };

  // Type of this token.
  [[no_unique_address]] Type type [[indeterminate]];

  constexpr Token(const Type t) noexcept : type(t) {}

  constexpr Token() noexcept { /*no init here!*/ };
};

#endif
