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
  void result(Token &result) noexcept;

private:
  /**
   * Move current position one token further.
   */
  void advance() noexcept;

  void expr_4(Token &result) noexcept;

  void expr_3(Token &result) noexcept;

  void expr_2(Token &result) noexcept;

  void expr_1(Token &result) noexcept;

  void expr_0(Token &result) noexcept;

  void number(Token &result) noexcept;

  void function(Token &result) noexcept;

  void constant(Token &result) noexcept;

  void subexpression(Token &result) noexcept;

  [[no_unique_address]] Token _current [[indeterminate]];
  [[no_unique_address]] Lexer &_lexer;
};

#endif
