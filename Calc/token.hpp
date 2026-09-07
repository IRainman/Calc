/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "identifiers.hpp"

struct Token {
  enum class Type : ParamCount {
    // https://en.cppreference.com/w/cpp/language/ascii
    RESULT = 0x00,

#ifdef CALC_USE_SEPARATORS
    SEPARATOR = ' ',
#endif

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
      [[no_unique_address]] unsigned int error_text_size [[indeterminate]];
      [[no_unique_address]] unsigned int error_position [[indeterminate]];
    };
#endif
  };

  // Type of this token.
  [[no_unique_address]] Type type [[indeterminate]];

#ifdef CALC_USE_SEPARATORS
  /**
   * Test whether this token can end a primary expression and therefore
   * participate in implicit multiplication.
   *
   * Examples:
   *
   *   2      -> true
   *   pi     -> true   (constant is represented as NUM)
   *   sqrt() -> true   (function result is represented as NUM)
   *   (...)  -> true
   */
  [[nodiscard]] constexpr bool ends_implicit_multiplication() const noexcept {
    return type == Type::NUM || type == Type::FUNCT || type == Type::RPAREN;
  }

  /**
   * Test whether this token can begin a primary expression and therefore
   * participate in implicit multiplication.
   *
   * Examples:
   *
   *   2pi      -> NUM NUM
   *   2sqrt(x) -> NUM FUNCT
   *   2(x)     -> NUM LPAREN
   */
  [[nodiscard]] constexpr bool begins_implicit_multiplication() const noexcept {
    return type == Type::NUM || type == Type::FUNCT || type == Type::LPAREN;
  }
#endif
};

#endif
