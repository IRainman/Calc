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

inline EquationSize Lexer::return_unparsable(Token &token) const noexcept {
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

[[nodiscard]] inline EquationSize
Lexer::read_operator(Token &token) const noexcept {
  [[assume((_view.size() >= 1))]];
  token.type = static_cast<Token::Type>(_view.front());
  return 1;
}

[[nodiscard]] inline EquationSize
Lexer::read_number(Token &token) const noexcept {
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
    const auto n = static_cast<EquationSize>(res.ptr - begin);
    return n;
  } else [[unlikely]] {
    // Handles the situation with a value that is either too small or too large
    // to parse correctly.
    return return_unparsable(token);
  }
}

[[nodiscard]] inline EquationSize
Lexer::read_ident(Token &token) const noexcept {
  [[assume((_view.size() >= 1))]];
  EquationSize n = 1;

  while (n != _view.size() &&
         ((_view[n] >= 'A' && _view[n] <= 'Z') ||
          (_view[n] >= 'a' && _view[n] <= 'z') ||
          (_view[n] >= '0' && _view[n] <= '9') || _view[n] == '_')) {
    ++n;
  }

  if (const auto i = ids.find(_view.substr(0, n)); i != ids.end()) [[likely]]
  {
    const auto &[caller, check] = i->second;
    if (check.is_constant()) {
      token.number = caller({});
      token.type = Token::Type::NUM;
    } else {
      token.function =
          &(*i); // Use the address of the iterator's dereferenced value
      token.type = Token::Type::FUNCT;
    }
    return n;
  } else [[unlikely]] {
    return return_unparsable(token);
  }
}

inline void Lexer::return_result(Token &token) const noexcept {
  token.type = Token::Type::RESULT;
}

void Lexer::next(Token &token) noexcept {
  while (!_view.empty()) [[likely]] {
    // https://en.cppreference.com/w/cpp/language/ascii
    const auto &cur = _view.front();
    if (cur == '%' || cur == '(' || cur == ')' || cur == '*' || cur == '+' ||
        cur == ',' || cur == '-' || cur == '/' || cur == '^') [[likely]] {
      advance(read_operator(token));
      return;
    }
    if ((cur >= '0' && cur <= '9')) [[likely]] {
      advance(read_number(token));
      return;
    }
    if ((cur >= 'A' && cur <= 'Z') || (cur >= 'a' && cur <= 'z')) [[likely]] {
      advance(read_ident(token));
      return;
    }
    if (cur == ' ') [[unlikely]] {
      advance(1);
      continue;
    }

    [[unlikely]] return_unparsable(token);
    return;
  }

  [[likely]] return_result(token);
  return;
}
