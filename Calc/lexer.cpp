/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#include "pch.hpp"

#include "lexer.hpp"

namespace {
const auto &ids = Identifiers::get();
};

[[nodiscard]] inline EquationSize Lexer::position() const noexcept {
  [[assume(_view.data() - _begin >= 0)]];

  return static_cast<EquationSize>(_view.data() - _begin);
}

inline void Lexer::advance(EquationSize n) noexcept { _view.remove_prefix(n); }

inline EquationSize Lexer::return_unparsable(Token &current) const noexcept {
  issue(current, position(), Issue::unparsable);
  return 0;
}

[[nodiscard]] inline EquationSize
Lexer::read_operator(Token &current) const noexcept {
  [[assume((_view.size() >= 1))]];

  current.type = static_cast<Token::Type>(_view.front());

  return 1;
}

[[nodiscard]] inline EquationSize Lexer::read_separator() const noexcept {
  [[assume((_view.size() >= 1))]];

  EquationSize n = 1;
  while (n != _view.size() && _view[n] == ' ') {
    ++n;
  }

  return n;
}

[[nodiscard]] inline EquationSize
Lexer::read_number(Token &current) const noexcept {
  [[assume((_view.size() >= 1))]];

  constexpr auto options =
      fast_float::parse_options{fast_float::chars_format::general
#ifndef FASTFLOAT_ONLY_POSITIVE_C_NUMBER_WO_INF_NAN
                                | fast_float::chars_format::no_infnan
#endif
      };

  const auto res = fast_float::from_chars_advanced(
      _view.data(), _view.data() + _view.size(), current.number, options);

  if (res.ec == std::errc{}) {
    [[assume(current.number >= 0 &&
             current.number <= std::numeric_limits<Value>::max())]];

    current.type = Token::Type::NUM;

    [[assume(res.ptr - _view.data() >= 1)]];
    return static_cast<EquationSize>(res.ptr - _view.data());

  } else [[unlikely]] {
    // Handles a value which is either too small or too large to parse.
    return return_unparsable(current);
  }
}

[[nodiscard]] inline EquationSize
Lexer::read_ident(Token &current) const noexcept {
  [[assume((_view.size() >= 1))]];

  EquationSize n = 1;
  while (n != _view.size() &&
         ((_view[n] >= 'A' && _view[n] <= 'Z') ||
          (_view[n] >= 'a' && _view[n] <= 'z') ||
          (_view[n] >= '0' && _view[n] <= '9') || _view[n] == '_')) {
    ++n;
  }

  if (const auto identifier = ids.find(_view.substr(0, n));
      identifier != ids.end()) {
    const auto &[_, c] = identifier->second;

    current.type = c.is_function() ? Token::Type::FUNCT : Token::Type::CONST;
    current.identifier = &(*identifier);

    return n;

  } else [[unlikely]] {
    return return_unparsable(current);
  }
}

inline void Lexer::return_result(Token &current) const noexcept {
  current.type = Token::Type::RESULT;
}

[[nodiscard]] inline constexpr static bool
implicit_mult_first(const Token::Type previous) noexcept {
  // clang-format off
  return previous == Token::Type::NUM ||
         previous == Token::Type::CONST ||
         previous == Token::Type::RPAREN;
  // clang-format on
}

[[nodiscard]] inline constexpr static bool
implicit_mult_second(const Token &current) noexcept {
  // clang-format off
  return current.type == Token::Type::NUM ||
         current.type == Token::Type::FUNCT ||
         current.type == Token::Type::CONST ||
         current.type == Token::Type::LPAREN;
  // clang-format on
}

void Lexer::next(Token &current) noexcept {
  if (_delayed.type != Token::Type::ISSUE) {
    current = _delayed;
    _delayed.type = Token::Type::ISSUE;
    _previous = current.type;
    return;
  }

  bool is_separator = false;

  while (!_view.empty()) {
    const auto &cur = _view.front();

    // https://en.cppreference.com/w/cpp/language/ascii
    [[assume(cur >= 32 && cur <= 126)]];

    if (cur == '(' || cur == ')' || cur == '*' || cur == '+' || cur == ',' ||
        cur == '-' || cur == '/' || cur == '^') {
      advance(read_operator(current));
      goto valid_token_return;

    } else if (cur >= '0' && cur <= '9') {
      advance(read_number(current));
      goto valid_token_return;

    } else if ((cur >= 'A' && cur <= 'Z') || (cur >= 'a' && cur <= 'z')) {
      advance(read_ident(current));
      goto valid_token_return;

    } else if (cur == ' ') {
      advance(read_separator());
      is_separator = true;
      continue;

    } else [[unlikely]] {
      // invalid_token_return
      return_unparsable(current);
      return;
    }
  valid_token_return:
    if (!is_separator && implicit_mult_first(_previous) &&
        implicit_mult_second(current)) {
      _delayed = current;
      current.type = Token::Type::MUL;
    }
    _previous = current.type;
    return;
  }

  return_result(current);
}
