/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#include "pch.hpp"

#include "issue_manager.hpp"
#include "parser.hpp"

namespace {
const auto &ids = Identifiers::get();
};

[[nodiscard]] Value Parser::parse() noexcept {
  const auto result = parse_expr_4();
  if (_current.type == Token::Type::END) [[likely]] {
    return result;
  }
  if (_current.type == Token::Type::ERROR) [[unlikely]] {
#ifdef CALC_USE_ERROR_TOKEN
    return error_position;
    return _current;
    ? needs to form nan with adress of an error.
#endif
  } else [[unlikely]] {
#ifdef CALC_USE_ERROR_TOKEN
    return error_position + extraneous input;
    return _current;
    ? needs to form nan with adress of an error.
#else
    IssueManager::report_error(_lex.get_position(), "extraneous input");
#endif
  }
#ifndef CALC_USE_ERROR_TOKEN
  [[unlikely]] return std::numeric_limits<Value>::quiet_NaN();
#endif
}

inline void Parser::advance() noexcept { _lex.next(_current); }

[[nodiscard]] Value Parser::parse_expr_4() noexcept {
  auto result = parse_expr_3();
  while (true) {
    switch (_current.type) {
    case Token::Type::ADD:
      advance();
      result += parse_expr_3();
      break;
    case Token::Type::SUB:
      advance();
      result -= parse_expr_3();
      break;
    default:
      return result;
    }
  }
}

[[nodiscard]] Value Parser::parse_expr_3() noexcept {
  auto result = parse_expr_2();
  while (true) {
    switch (_current.type) {
    case Token::Type::MUL:
      advance();
      result *= parse_expr_2();
      break;
    case Token::Type::DIV:
      advance();
      result /= parse_expr_2();
      break;
    default:
      return result;
    }
  }
}

[[nodiscard]] Value Parser::parse_expr_2() noexcept {
  std::array<Value, std::numeric_limits<ParamCount>::max()> values
      [[indeterminate]];

  ParamCount count = 0;
  do {
    values[count] = parse_expr_1();

    if (_current.type == Token::Type::POW) {
      advance();
    } else {
      break;
    }
  } while (++count != static_cast<ParamCount>(values.size()));

  if (count == static_cast<ParamCount>(values.size())) [[unlikely]] {
#ifdef CALC_USE_ERROR_TOKEN
    constexpr static auto err_str = "too many ^ in expression";
    _current.error_text = err_str;
    _current.error_position = _lex.get_position();
    return _current;
    ? needs to form nan with adress of an error.
#else
    IssueManager::report_error(_lex.get_position(), "too many ^ in expression");
    return _current.number;
#endif
  }

  Value &result = values[count];
  while (--count != static_cast<ParamCount>(-1)) {
    result = Identifiers::pow(values[count], result);
  };
  return result;
}

[[nodiscard]] Value Parser::parse_expr_1() noexcept {
  switch (_current.type) {
  case Token::Type::SUB:
    advance();
    return -parse_expr_0();
  default:
    return parse_expr_0();
  }
}

[[nodiscard]] Value Parser::parse_expr_0() noexcept {
  switch (_current.type) {
  case Token::Type::LPAREN:
    [[likely]] {
      advance();
      const auto result = parse_expr_4();
      if (_current.type == Token::Type::RPAREN) [[likely]] {
        advance();
        return result;
      } else [[unlikely]] {
#ifdef CALC_USE_ERROR_TOKEN
        constexpr static auto err_str = "expected parenthesis";
        _current.error_text = err_str;
        _current.error_position = _lex.get_position();
        return _current;
        ? needs to form nan with adress of an error.
#else
        IssueManager::report_error(_lex.get_position(), "expected parenthesis");
        return _current.number;
#endif
      }
    }
  case Token::Type::NUM:
    [[likely]] {
      const auto num = _current.number;
      advance();
      return num;
    }
  case Token::Type::FUNCT:
    [[likely]] { return parse_function(); }
  default:
    [[unlikely]] {
#ifdef CALC_USE_ERROR_TOKEN
      constexpr static auto err_str = "unexpected";
      _current.error_text = err_str;
      _current.error_position = _lex.get_position();
      return _current;
      ? needs to form nan with adress of an error.
#else
      IssueManager::report_error(_lex.get_position(), "unexpected");
      return _current.number;
#endif
    }
  }
}

[[nodiscard]] Value Parser::parse_function() noexcept {
  auto function_start_pos = _lex.get_position();
  const auto i = _current.function;

  advance();
  if (_current.type == Token::Type::LPAREN) [[likely]] {
    advance();

    const auto [check, function] = i->second;

    std::array<Value, std::numeric_limits<ParamCount>::max()> parameters
        [[indeterminate]];
    ParamCount count = 0;

    do {
      parameters[count] = parse_expr_4();
      ++count;

      switch (_current.type) {
      case Token::Type::RPAREN:
        [[likely]] {
          advance();
          if (check.params_count_is_valid(count)) [[likely]] {
            return function({parameters.begin(), parameters.begin() + count});
          } else [[unlikely]] {
            function_start_pos -= i->first.size();
#ifdef CALC_USE_ERROR_TOKEN
            constexpr static auto err_str = "incorrect parameters count";
            _current.error_text = err_str;
            _current.error_position = function_start_pos;
            return _current;
            ? needs to form nan with adress of an error.
#else
            IssueManager::report_error(function_start_pos,
                                       "incorrect parameters count");
            return _current.number;
#endif
          }
        }
      case Token::Type::COMA:
        [[likely]] {
          advance();
          continue;
        }
      default:
        [[unlikely]] {
#ifdef CALC_USE_ERROR_TOKEN
          constexpr static auto err_str = "expected parenthesis";
          _current.error_text = err_str;
          _current.error_position = _lex.get_position();
          return _current;
          ? needs to form nan with adress of an error.
#else
          IssueManager::report_error(_lex.get_position(),
                                     "expected parenthesis");
          return _current.number;
#endif
        }
      }
    } while (count != static_cast<ParamCount>(parameters.size()));
    [[likely]]

#ifdef CALC_USE_ERROR_TOKEN
    constexpr static auto err_str = "too many parameters";
    _current.error_text = err_str;
    _current.error_position = _lex.get_position();
    return _current;
    ? needs to form nan with adress of an error.
#else
    IssueManager::report_error(_lex.get_position(), "too many parameters");
    return _current.number;
#endif
  } else [[unlikely]] {
#ifdef CALC_USE_ERROR_TOKEN
    constexpr static auto err_str = "expected parenthesis";
    _current.error_text = err_str;
    _current.error_position = _lex.get_position();
    return _current;
    ? needs to form nan with adress of an error.
#else
    IssueManager::report_error(_lex.get_position(), "expected parenthesis");
    return _current.number;
#endif
  }
}
