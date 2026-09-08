/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#include "pch.hpp"

#include "issue_manager.hpp"
#include "lexer.hpp"

namespace {
const auto &ids = Identifiers::get();
};

[[nodiscard]] EquationSize Lexer::position() const noexcept {
  [[assume(_view.data() - _begin >= 0)]];
  return static_cast<EquationSize>(_view.data() - _begin);
}

void Lexer::advance(EquationSize n) noexcept { _view.remove_prefix(n); }

EquationSize Lexer::return_unparsable(Token &token) const noexcept {
#ifdef CALC_USE_ERROR_TOKEN
  constexpr std::string_view err = "unparsable";
  token.error_text = err.data();
  token.error_text_size = err.size();
  token.error_position = position();
#else
  IssueManager::report_error(position(), "unparsable");
#endif
  token.type = Token::Type::ERROR;
  return 0;
}

[[nodiscard]] EquationSize Lexer::read_operator(Token &token) const noexcept {
  [[assume((_view.size() >= 1))]];
  token.type = static_cast<Token::Type>(_view.front());
  return 1;
}

[[nodiscard]] EquationSize Lexer::read_separator(
#ifdef CALC_USE_SEPARATORS
    Token &token
#endif
) const noexcept {
  [[assume((_view.size() >= 1))]];

  EquationSize n = 1;

  while (n != _view.size() && _view[n] == ' ') {
    ++n;
  }

#ifdef CALC_USE_SEPARATORS
  token.type = static_cast<Token::Type>(_view.front());
#endif

  return n;
}

[[nodiscard]] EquationSize Lexer::read_number(Token &token) const noexcept {
  [[assume((_view.size() >= 1))]];

  const auto begin = _view.data();
  const auto end = _view.data() + _view.size();

  constexpr auto opt =
      fast_float::parse_options{fast_float::chars_format::general
#ifndef FASTFLOAT_ONLY_POSITIVE_C_NUMBER_WO_INF_NAN
                                | fast_float::chars_format::no_infnan
#endif
      };

  const auto res =
      fast_float::from_chars_advanced(begin, end, token.number, opt);

  if (res.ec == std::errc{}) [[likely]] {
    [[assume(token.number >= 0 &&
             token.number <= std::numeric_limits<Value>::max())]];

    token.type = Token::Type::NUM;

    [[assume(res.ptr - begin >= 1)]];
    return static_cast<EquationSize>(res.ptr - begin);

  } else [[unlikely]] {
    /*
     * Handles a value which is either too small or too large to parse
     * correctly.
     */
    return return_unparsable(token);
  }
}

[[nodiscard]] EquationSize Lexer::read_ident(Token &token) const noexcept {
  [[assume((_view.size() >= 1))]];

  EquationSize n = 1;

  while (n != _view.size() &&
         ((_view[n] >= 'A' && _view[n] <= 'Z') ||
          (_view[n] >= 'a' && _view[n] <= 'z') ||
          (_view[n] >= '0' && _view[n] <= '9') || _view[n] == '_')) {
    ++n;
  }

  if (const auto i = ids.find(_view.substr(0, n)); i != ids.end()) [[likely]] {
    const auto &[caller, check] = i->second;

    if (check.is_constant()) {
      token.number = caller({});
      token.type = Token::Type::NUM;
    } else {
      token.function = &(*i);
      token.type = Token::Type::FUNCT;
    }

    return n;

  } else [[unlikely]] {
    return return_unparsable(token);
  }
}

void Lexer::return_result(Token &token) const noexcept {
  token.type = Token::Type::RESULT;
}

/**
 * Lex one token without performing any implicit-multiplication processing.
 *
 * This function deliberately contains the original lexical rules. Keeping
 * it separate makes the implicit-multiplication layer small and predictable.
 */
void Lexer::next_raw(Token &token) noexcept {
  while (!_view.empty()) [[likely]] {
    const auto &cur = _view.front();

    // https://en.cppreference.com/w/cpp/language/ascii
    [[assume(cur >= 32 && cur <= 126)]];

    if (cur == '(' || cur == ')' || cur == '*' || cur == '+' || cur == ',' ||
        cur == '-' || cur == '/' || cur == '^') {

      advance(read_operator(token));
      return;

    } else if (cur >= '0' && cur <= '9') {

      advance(read_number(token));
      return;

    } else if ((cur >= 'A' && cur <= 'Z') || (cur >= 'a' && cur <= 'z')) {

      advance(read_ident(token));
      return;

    } else if (cur == ' ') {

      advance(read_separator(
#ifdef CALC_USE_SEPARATORS
          token
#endif
          ));
#ifdef CALC_USE_SEPARATORS
      return;
#else
      continue;
#endif

    } else [[unlikely]] {

      return_unparsable(token);
      return;
    }
  }

  [[likely]] return_result(token);
}

void Lexer::next(Token &token) noexcept {
#ifndef CALC_USE_SEPARATORS

  /*
   * Without separator tokens we cannot distinguish:
   *
   *     2pi
   *
   * from:
   *
   *     2 pi
   *
   * Therefore implicit multiplication must not be generated in this mode.
   */
  next_raw(token);
  return;

#else

  /*
   * Return a token which was lexed during the previous call.
   */
  if (_has_pending) [[unlikely]] {
    token = _pending;
    _has_pending = false;

    _previous = token.type;
    _has_previous = true;
    return;
  }

  Token current [[indeterminate]];
  next_raw(current);

  /*
   * The first token can never require implicit multiplication.
   */
  if (!_has_previous) [[likely]] {
    token = current;
    _previous = token.type;
    _has_previous = true;
    return;
  }

  /*
   * Whitespace is itself a token. Since _previous is SEPARATOR after a
   * whitespace run, implicit multiplication is automatically suppressed.
   */
  if (_previous == Token::Type::SEPARATOR) [[unlikely]] {
    token = current;
    _previous = token.type;
    return;
  }

  /*
   * Insert an explicit multiplication operator between adjacent primary
   * expressions:
   *
   *     2pi       -> 2 * pi
   *     2(3)      -> 2 * (3)
   *     (2)(3)    -> (2) * (3)
   *
   * A function followed by '(' is deliberately excluded:
   *
   *     sqrt(4)   -> sqrt(4)
   */
  if (can_end_primary(_previous) && can_start_primary(current.type) &&
      !is_function_call(_previous, current.type)) [[unlikely]] {

    _pending = current;
    _has_pending = true;

    token.type = Token::Type::MUL;

    /*
     * MUL becomes the previous token so that the pending operand does not
     * cause another implicit MUL on the next call.
     */
    _previous = Token::Type::MUL;

    return;
  }

  token = current;
  _previous = token.type;

#endif
}
