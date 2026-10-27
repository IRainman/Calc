/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#include "pch.hpp"

#include "lexer.hpp"

namespace {
const auto &ids = Identifiers::get();
};

inline EquationPosition Lexer::position() const noexcept {
  [[assume(_current - _begin >= 0 && _current < _end)]];

  return static_cast<EquationPosition>(_current - _begin);
}

inline void Lexer::return_unparsable(Token &current) const noexcept {
  current.position = position();
  issue(current, Issue::unparsable);
}

inline void Lexer::read_operator(Token &current) noexcept {
  [[assume(_current - _begin >= 1 && _current < _end)]];

  current.position = position();
  current.type = static_cast<Token::Type>(*_current);

  ++_current;

  [[assume(_current - _begin >= 1 && _current <= _end)]];
}

inline void Lexer::read_separator(Token & /*current*/) noexcept {
  [[assume((_current - _begin >= 1 && _current < _end))]];
#if 0
  current.position = position();
  current.type = static_cast<Token::Type>(*_current);
#endif
  do {
    ++_current;
  } while (_current != _end && *_current == ' ');

  [[assume(_current - _begin >= 1 && _current <= _end)]];
}

[[nodiscard]] inline void Lexer::read_number(Token &current) noexcept {
  [[assume(_current - _begin >= 1 && _current < _end)]];

  current.position = position();

  constexpr auto options =
      fast_float::parse_options{fast_float::chars_format::general
#ifndef FASTFLOAT_ONLY_POSITIVE_C_NUMBER_WO_INF_NAN
                                | fast_float::chars_format::no_infnan
#endif
      };

  const auto res =
      fast_float::from_chars_advanced(_current, _end, current.number, options);

  if (res.ec == std::errc{}) {
    [[assume(current.number >= 0 &&
             current.number <= std::numeric_limits<Value>::max())]];

    current.type = Token::Type::NUM;
    _current = res.ptr;

    [[assume(_current - _begin >= 1 && _current <= _end)]];
  } else [[unlikely]] {
    // Handles a value which is either too small or too large to parse.
    return_unparsable(current);
  }
}

[[nodiscard]] inline void Lexer::read_ident(Token &current) noexcept {
  [[assume(_current - _begin >= 1 && _current < _end)]];

  current.position = position();

  const auto ident_begin = _current;

  do {
    ++_current;
  } while (_current != _end &&
           ((*_current >= 'A' && *_current <= 'Z') ||
            (*_current >= 'a' && *_current <= 'z') ||
            (*_current >= '0' && *_current <= '9') || *_current == '_'));

  if (const auto identifier = ids.find(std::string_view(ident_begin, _current));
      identifier != ids.end()) {
    const auto &[_, p] = identifier->second;
    current.identifier = &(*identifier);
    current.type = p.is_function() ? Token::Type::FUNCT : Token::Type::CONST;
  } else [[unlikely]] {
    return_unparsable(current);
  }
  [[assume(_current - _begin >= 1 && _current <= _end)]];
}

inline void Lexer::return_result(Token &current) const noexcept {
  current.position = position();
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
    _previous = _delayed.type;
    _delayed.type = Token::Type::ISSUE;
    return;
  }

  bool is_separator = false;

  while (_current != _end) {
    // https://en.cppreference.com/w/cpp/language/ascii
    [[assume(*_current >= 32 && *_current <= 126)]];

    if (*_current == '(' || *_current == ')' || *_current == '*' ||
        *_current == '+' || *_current == ',' || *_current == '-' ||
        *_current == '/' || *_current == '^') {
      read_operator(current);
      goto valid_token_return;

    } else if (*_current >= '0' && *_current <= '9') {
      read_number(current);

      goto valid_token_return;

    } else if ((*_current >= 'A' && *_current <= 'Z') ||
               (*_current >= 'a' && *_current <= 'z')) {
      read_ident(current);
      goto valid_token_return;

    } else if (*_current == ' ') {
      read_separator(current);
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
