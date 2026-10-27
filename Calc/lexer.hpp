/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef LEXER_HPP
#define LEXER_HPP
#include "token.hpp"

class Lexer {
public:
  explicit Lexer(const std::string_view &data) noexcept
      : _begin{data.data()}, _end{data.data() + data.size()} {
    _current = _begin;
  }

  Lexer() = delete;
  Lexer(const Lexer &) = delete;
  Lexer(Lexer &&) = delete;

  /**
   * Write the next token to the link.
   */
  void next(Token &token) noexcept;

private:
  /**
   * Return the current position.
   */
  [[nodiscard]] EquationPosition position() const noexcept;

  /**
   * Read a separator at the beginning of the input view.
   */
  void read_separator(Token &token) noexcept;

  /**
   * Read an operator at the beginning of the input view.
   */
  void read_operator(Token &token) noexcept;

  /**
   * Read a number at the beginning of the input view.
   */
  void read_number(Token &token) noexcept;

  /**
   * Read an identifier at the beginning of the input view.
   */
  void read_ident(Token &token) noexcept;

  /**
   * Read the normal end at the beginning of the input view.
   */
  void return_result(Token &token) const noexcept;

  /**
   * Read some unparsable input in the input view.
   */
  void return_unparsable(Token &current) const noexcept;

  
  const std::string_view::const_pointer _begin;
  const std::string_view::const_pointer _end;
  std::string_view::const_pointer _current;
  [[no_unique_address]] Token _delayed{Token::Type::ISSUE};
  [[no_unique_address]] Token::Type _previous = Token::Type::ISSUE;
};

#endif
