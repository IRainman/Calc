/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef PARSER_HPP
#define PARSER_HPP

#include "lexer.hpp"

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
   *        | CONST
   *        | FUNCT
   *        ;
   *
   * function = FUNCT '(' parameters ')'
   *
   * parameters = expr_4 ( ',' expr_4 )* ')'
   */
  explicit Parser(Lexer &lexer) noexcept : _lexer(lexer) {}

  Parser() = delete;
  Parser(const Parser &) = delete;
  Parser(Parser &&) = delete;

  /**
   * Parse and evaluate an expression.
   */
  [[nodiscard]] Token result() noexcept;

private:
  /**
   * Move current position one token further.
   */
  void advance() noexcept;

  [[nodiscard]] Token expr_4() noexcept;

  [[nodiscard]] Token expr_3() noexcept;

  [[nodiscard]] Token expr_2() noexcept;

  [[nodiscard]] Token expr_1() noexcept;

  [[nodiscard]] Token expr_0() noexcept;

  [[nodiscard]] Token number() noexcept;

  [[nodiscard]] Token function() noexcept;

  [[nodiscard]] Token constant() noexcept;

  [[nodiscard]] Token subexpression() noexcept;

  [[no_unique_address]] Lexer &_lexer;
  [[no_unique_address]] Token _current [[indeterminate]];
};

#endif
