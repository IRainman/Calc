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
  Lexer(Lexer &&) = default;

private:
  /**
   * Write the next token to the link.
   */
  void next(Token &token) noexcept;

  /**
   * Return the current position.
   */
  [[nodiscard]] EquationSize get_position() const noexcept;

  /**
   * Move current position n symbols further.
   */
  void advance(EquationSize n) noexcept;

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
  void read_end(Token &token) const noexcept;

  /**
   * Read an some unparsable in the input view and return 0 to reparse from this
   * point.
   */
  [[nodiscard]] EquationSize read_unparsable(Token &token) const noexcept;

  std::string_view _view;
  const std::string_view::const_pointer _begin;
  friend class Parser;
};
#endif
