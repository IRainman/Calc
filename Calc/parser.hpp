/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef PARSER_HPP
#define PARSER_HPP

#include "lexer.hpp"

#ifdef CALC_USE_ERROR_TOKEN
typedef Token Result;
#else
typedef Value Result;
#endif

class Parser {
public:
  /**
   * This is a recursive descent parser with the following grammar:
   *
   * expr   = expr_4 <RESULT>
   *
   * expr_4 = expr_3 ( ('+' | '-') expr_3 )*;
   *
   * expr_3 = expr_2 ( ('*' | '/') expr_2 )*
   *
   * expr_2 = expr_1 ( '^' expr_1 )*;
   *
   * expr_1 = expr_0 | '-' expr_0 | expr_0;
   *
   * expr_0 = '(' expr_4 ')'
   *        | NUM
   *        | FUNCT
   *        ;
   *
   * parse_function = FUNCT '(' parameters ')'
   *
   * parameters = expr_4 ( ',' expr_4 )* ')'
   */
  explicit  Parser(Lexer &lexer) noexcept : _lexer(lexer) {
    advance();
  }

  Parser() = delete;
  Parser(const Parser &) = delete;
   Parser(Parser &&) = default;

  /**
   * Parse and evaluate an expression.
   */
  [[nodiscard]]  Result parse() noexcept;

private:
  /**
   * Move current position one token further.
   */
  void advance() noexcept;

  [[nodiscard]]  Result parse_expr_4() noexcept;

  [[nodiscard]]  Result parse_expr_3() noexcept;

  [[nodiscard]]  Result parse_expr_2() noexcept;

  [[nodiscard]]  Result parse_expr_1() noexcept;

  [[nodiscard]]  Result parse_expr_0() noexcept;

  [[nodiscard]]  Result parse_function() noexcept;

  [[no_unique_address]] Lexer &_lexer;
  [[no_unique_address]] Token _current [[indeterminate]];
};

#endif
