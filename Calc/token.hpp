/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "identifiers.hpp"

// clang-format off
constexpr static const char *unparsable = "unparsable";

constexpr static const char *extraneous_input = "extraneous input";

constexpr static const char *to_many_in_expression = "too many ^ in expression";

constexpr static const char *expected_parenthesis = "expected parenthesis";

constexpr static const char *incorrect_parameters_count = "incorrect parameters count";

constexpr static const char *too_many_parameters = "too many parameters";

constexpr static const char *unexpected = "unexpected";
// clang-format on

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

    ERROR = 0x7F,
  };

  union {
    // If token is a number parsed from string or a constant, this is the value
    // of it.
    [[no_unique_address]] Value number [[indeterminate]];

    // If token is a function, there is a pointer to it.
    [[no_unique_address]] Identifiers::map::const_pointer function
        [[indeterminate]];

#ifdef CALC_USE_ERROR_TOKEN
    // If token is an error, there is a small helper that handle it.
    struct {
      [[no_unique_address]] const char *error_text [[indeterminate]];
      [[no_unique_address]] unsigned int error_position [[indeterminate]];
      [[no_unique_address]] unsigned char error_text_size [[indeterminate]];
    };
#endif
  };

  // Type of this token.
  [[no_unique_address]] Type type [[indeterminate]];

  constexpr explicit Token(const Type t) noexcept : type(t) {}

  Token() noexcept { /*no init here!*/ };
};

#endif
