/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#include "pch.hpp"

#include "issue_manager.hpp"
#include "parser.hpp"

namespace {
const auto &ids = Identifiers::get();
};

[[nodiscard]]
#ifdef CALC_USE_ERROR_TOKEN
Token
#else
Value
#endif
     Parser::parse() noexcept {
  const auto result = parse_expr_4();

#ifdef CALC_USE_ERROR_TOKEN
  switch (_current.type) {
  case Token::Type::RESULT:
    [[likely]] return result;
  case Token::Type::ERROR:
    [[unlikely]] return _current;
  default:
    [[unlikely]] _current.type = Token::Type::ERROR;
    _current.error_text = "extraneous input";
    return _current;
  }
#else
  switch (_current.type) {
  case Token::Type::RESULT:
    [[likely]] return result;
  case Token::Type::ERROR:
  [[unlikely]]
  default:
    [[unlikely]] IssueManager::report_error(_lex.position(),
                                            "extraneous input");
    break;
  }
  return std::numeric_limits<Value>::quiet_NaN();
#endif
}

inline void Parser::advance() noexcept { _lex.next(_current); }

[[nodiscard]]
#ifdef CALC_USE_ERROR_TOKEN
Token
#else
Value
#endif
Parser::parse_expr_4() noexcept {
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

[[nodiscard]]
#ifdef CALC_USE_ERROR_TOKEN
Token
#else
Value
#endif
Parser::parse_expr_3() noexcept {
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

[[nodiscard]]
#ifdef CALC_USE_ERROR_TOKEN
Token
#else
    Value
#endif
    Parser::parse_expr_2() noexcept {
  std::array<
#ifdef CALC_USE_ERROR_TOKEN
      Token
#else
      Value
#endif
      ,
      std::numeric_limits<ParamCount>::max()>
      values [[indeterminate]];

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
    _current.error_text = "too many ^ in expression";
    _current.error_position = _lex.position();
    return _current;
#else
    IssueManager::report_error(_lex.position(), "too many ^ in expression");
    return _current.number;
#endif
  }

#ifdef CALC_USE_ERROR_TOKEN
  Token
#else
  Value
#endif
      &result = values[count];
  while (--count != static_cast<ParamCount>(-1)) {
    result
#ifdef CALC_USE_ERROR_TOKEN
        .number
#endif
        = Identifiers::pow(values[count]
#ifdef CALC_USE_ERROR_TOKEN
                               .number
#endif
                           ,
                           result
#ifdef CALC_USE_ERROR_TOKEN
                               .number
#endif

        );
  };

  return result;
}

[[nodiscard]]
#ifdef CALC_USE_ERROR_TOKEN
Token
#else
Value
#endif
Parser::parse_expr_1() noexcept {
  switch (_current.type) {
  case Token::Type::SUB:
    advance();
    return -parse_expr_0();
  default:
    return parse_expr_0();
  }
}

[[nodiscard]]
#ifdef CALC_USE_ERROR_TOKEN
Token
#else
Value
#endif
Parser::parse_expr_0() noexcept {
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
        _current.error_position = _lex.position();
        return _current;
        ? needs to form nan with adress of an error.
#else
        IssueManager::report_error(_lex.position(), "expected parenthesis");
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
      _current.error_position = _lex.position();
      return _current;
      ? needs to form nan with adress of an error.
#else
      IssueManager::report_error(_lex.position(), "unexpected");
      return _current.number;
#endif
    }
  }
}

[[nodiscard]]
#ifdef CALC_USE_ERROR_TOKEN
Token
#else
Value
#endif
Parser::parse_function() noexcept {
  auto function_start_pos = _lex.position();
  const auto i = _current.function;

  advance();
  if (_current.type == Token::Type::LPAREN) [[likely]] {
    advance();

    const auto &[caller, check] = i->second;

    std::array<
#ifdef CALC_USE_ERROR_TOKEN
        Token
#else
        Value
#endif
        ,
        std::numeric_limits<ParamCount>::max()>
        parameters [[indeterminate]];
    ParamCount count = 0;

    do {
      parameters[count] = parse_expr_4();
      ++count;

      switch (_current.type) {
      case Token::Type::RPAREN:
        [[likely]] {
          advance();
          if (check.is_function() && check.params_count_is_valid(count))
              [[likely]] {
#ifdef CALC_USE_ERROR_TOKEN
            _current.type = Token::Type::NUM;
            _current.number =
                caller({parameters.begin(), parameters.begin() + count});
            return _current;
#else
            return caller({parameters.begin(), parameters.begin() + count});
#endif
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
          _current.error_position = _lex.position();
          return _current;
          ? needs to form nan with adress of an error.
#else
          IssueManager::report_error(_lex.position(),
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
    _current.error_position = _lex.position();
    return _current;
    ? needs to form nan with adress of an error.
#else
    IssueManager::report_error(_lex.position(), "too many parameters");
    return _current.number;
#endif
  } else [[unlikely]] {
#ifdef CALC_USE_ERROR_TOKEN
    constexpr static auto err_str = "expected parenthesis";
    _current.error_text = err_str;
    _current.error_position = _lex.position();
    return _current;
    ? needs to form nan with adress of an error.
#else
    IssueManager::report_error(_lex.position(), "expected parenthesis");
    return _current.number;
#endif
  }
}
