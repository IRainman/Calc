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
   *
   * When CALC_USE_SEPARATORS is enabled, adjacent primary expressions are
   * converted into an explicit MUL token by the lexer. Whitespace prevents
   * this transformation.
   */
  void next(Token &token) noexcept;

  /**
   * Read the next lexical token without applying implicit multiplication.
   */
  void next_raw(Token &token) noexcept;

  /**
   * Return the current position.
   */
  [[nodiscard]] EquationSize position() const noexcept;

  /**
   * Move current position n symbols further.
   */
  void advance(EquationSize n) noexcept;

#ifdef CALC_USE_SEPARATORS
  /**
   * Return whether a token can terminate a primary expression.
   */
  [[nodiscard]] static bool can_end_primary(const Token::Type type) noexcept {
    return type == Token::Type::NUM || type == Token::Type::FUNCT ||
           type == Token::Type::RPAREN;
  }

  /**
   * Return whether a token can start a primary expression.
   */
  [[nodiscard]] static bool can_start_primary(const Token::Type type) noexcept {
    return type == Token::Type::NUM || type == Token::Type::FUNCT ||
           type == Token::Type::LPAREN;
  }

  /**
   * Return whether two adjacent tokens form a function call rather than
   * implicit multiplication.
   */
  [[nodiscard]] static bool
  is_function_call(const Token::Type previous,
                   const Token::Type current) noexcept {
    return previous == Token::Type::FUNCT && current == Token::Type::LPAREN;
  }
#endif

  /**
   * Read a separator at the beginning of the input view and return its size.
   */
  [[nodiscard]] EquationSize read_separator() const noexcept;

  /**
   * Read an operator at the beginning of the input view and return its size.
   */
  [[nodiscard]] EquationSize read_operator(Token &token) const noexcept;

  /**
   * Read a number at the beginning of the input view and return its size.
   */
  [[nodiscard]] EquationSize read_number(Token &token) const noexcept;

  /**
   * Read an identifier at the beginning of the input view and return its
   * size.
   */
  [[nodiscard]] EquationSize read_ident(Token &token) const noexcept;

  /**
   * Read the normal end at the beginning of the input view.
   */
  void return_result(Token &token) const noexcept;

  /**
   * Read some unparsable input and return 0 to reparse from this point.
   */
  EquationSize return_unparsable(Token &token) const noexcept;

  std::string_view _view;
  const std::string_view::const_pointer _begin;

#ifdef CALC_USE_SEPARATORS
  /**
   * Token which was already lexed but has not yet been returned because an
   * implicit multiplication token must be emitted before it.
   */
  Token _pending [[indeterminate]];

  /**
   * True when _pending contains a token waiting to be returned.
   */
  bool _has_pending = false;

  /**
   * Last token returned to the parser.
   */
  Token::Type _previous = Token::Type::RESULT;

  /**
   * True after at least one token has been returned.
   */
  bool _has_previous = false;
#endif

  friend class Parser;
};

#endif
