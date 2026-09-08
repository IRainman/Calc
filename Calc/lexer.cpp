/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#include "pch.hpp"

#include "issue_manager.hpp"
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
#ifdef CALC_USE_ERROR_TOKEN
  current.error_text = unparsable;
  current.error_text_size = 11;
  current.error_position = position();
#else
  IssueManager::report_error(position(), unparsable);
#endif
  current.type = Token::Type::ERROR;

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

  const auto begin = _view.data();
  const auto end = _view.data() + _view.size();

  constexpr auto options =
      fast_float::parse_options{fast_float::chars_format::general
#ifndef FASTFLOAT_ONLY_POSITIVE_C_NUMBER_WO_INF_NAN
                                | fast_float::chars_format::no_infnan
#endif
      };

  const auto res =
      fast_float::from_chars_advanced(begin, end, current.number, options);

  if (res.ec == std::errc{}) [[likely]] {
    [[assume(current.number >= 0 &&
             current.number <= std::numeric_limits<Value>::max())]];

    current.type = Token::Type::NUM;

    [[assume(res.ptr - begin >= 1)]];
    return static_cast<EquationSize>(res.ptr - begin);

  } else [[unlikely]] {
    /*
     * Handles a value which is either too small or too large to parse
     * correctly.
     */
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

  if (const auto i = ids.find(_view.substr(0, n)); i != ids.end()) [[likely]] {

    const auto &[caller, check] = i->second;

    if (check.is_constant()) {
      current.number = caller({});
      current.type = Token::Type::NUM;
    } else {
      current.function = &(*i);
      current.type = Token::Type::FUNCT;
    }

    return n;

  } else [[unlikely]] {
    return return_unparsable(current);
  }
}

inline void Lexer::return_result(Token &current) const noexcept {
  current.type = Token::Type::RESULT;
}

[[nodiscard]] inline constexpr static bool
operand_for_implicit_multiplication(const Token::Type previous) noexcept {
  // clang-format off
  return previous == Token::Type::NUM ||
         previous == Token::Type::RPAREN;
  // clang-format on
}

[[nodiscard]] inline constexpr static bool
operand_for_implicit_multiplication(const Token &current) noexcept {
  // clang-format off
  return current.type == Token::Type::NUM ||
         current.type == Token::Type::FUNCT ||
         current.type == Token::Type::LPAREN;
  // clang-format on
}

[[nodiscard]] inline constexpr static bool
function_call(const Token::Type previous, const Token &current) noexcept {
  // clang-format off
  return previous == Token::Type::FUNCT &&
     current.type == Token::Type::LPAREN;
  // clang-format on
}

void Lexer::next(Token &current) noexcept {
#ifdef CALC_ALLOW_IMPLICIT_MULTIPLICATION
  if (_pending.type != Token::Type::ERROR) [[unlikely]] {
    current = _pending;
    _pending.type = Token::Type::ERROR;
    _previous = current.type;
    return;
  }
  bool separator = false;
#endif

  while (!_view.empty()) [[likely]] {

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
#ifdef CALC_ALLOW_IMPLICIT_MULTIPLICATION
      separator = true;
#endif
      continue;
    } else [[unlikely]] {
      // invalid_token_return
      return_unparsable(current);
      return;
    }
  valid_token_return:
#ifdef CALC_ALLOW_IMPLICIT_MULTIPLICATION
    // clang-format off
    if (!separator && 
        !function_call(_previous, current) &&
        operand_for_implicit_multiplication(_previous) &&
        operand_for_implicit_multiplication(current))
      [[unlikely]] {
      // clang-format on
      _pending = current;
      current.type = Token::Type::MUL;
    }
    _previous = current.type;
#endif
    return;
  }

  [[likely]] return_result(current);
}
