/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef LEXER_HPP
#define LEXER_HPP
#include "token.hpp"

class Lexer {
public:
  explicit Lexer(const std::string_view data) noexcept
      : _view{data}, _begin{data.data()} {}

  Lexer() = delete;
  Lexer(const Lexer &) = delete;
  Lexer(Lexer &&) = delete;

private:
  /**
   * Write the next token to the link.
   */
  void next(Token &token) noexcept;

  /**
   * Return the current position.
   */
  [[nodiscard]] EquationSize position() const noexcept;

  /**
   * Move current position n symbols further.
   */
  void advance(EquationSize n) noexcept;

  /**
   * Read a separator at the beginning of the input view and return its size.
   */
  [[nodiscard]] EquationSize read_separator() const noexcept;

  /**
   * Read an operator at the beginning of the input view and return size of it.
   */
  [[nodiscard]] EquationSize read_operator(Token &token) const noexcept;

  /**
   * Read a number at the beginning of the input view and return size of it.
   */
  [[nodiscard]] EquationSize read_number(Token &token) const noexcept;

  /**
   * Read an identifier at the beginning of the input view and return size of
   * it.
   */
  [[nodiscard]] EquationSize read_ident(Token &token) const noexcept;

  /**
   * Read the normal end at the beginning of the input view.
   */
  void return_result(Token &token) const noexcept;

  /**
   * Read some unparsable input in the input view and return 0 to reparse from
   * this point.
   */
  EquationSize return_unparsable(Token &token) const noexcept;

  std::string_view _view;
  const std::string_view::const_pointer _begin;
#ifdef CALC_ALLOW_IMPLICIT_MULTIPLICATION
  Token _pending{Token::Type::ERROR};
  Token::Type _previous = Token::Type::ERROR;
#endif
  friend class Parser;
};

#endif
