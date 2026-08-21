#pragma once
/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#include "identifiers.hpp"

struct Token {
  enum class Type : ParamCount {
    // https://en.cppreference.com/w/cpp/language/ascii
    END = 0x00,

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
    [[no_unique_address]] Identifiers::map::const_pointer function[[indeterminate]];

#ifdef CALC_USE_ERROR_TOKEN
    // If token is an error, there is a small helper that handle it.
    struct {
      const char *error_text [[indeterminate]];
      EquationSize error_position [[indeterminate]];
    };
#endif
  };

  // Type of this token.
  [[no_unique_address]] Type type [[indeterminate]];
};
