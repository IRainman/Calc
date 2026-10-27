/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#include "pch.hpp"

#include "parser.hpp"

namespace {
const auto &ids = Identifiers::get();
};

// clang-format off
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4061) // enumerator in switch of enum not explicitly handled
#pragma warning(disable : 4062) // enumerator in switch of enum not handled
#endif
// clang-format on

void Parser::result(Token &result) noexcept {
  advance();
  expr_4(result);
  switch (_current.type) {
  case Token::Type::RESULT: {
    // full expression parsed, may produce RESULT or ISSUE
    return;
  }
  case Token::Type::ISSUE: {
    // early exit with grammar error
    [[unlikely]] result = _current;
    return;
  }
  default: {
    // early exit when expression is incorrect
    result.position = _current.position;
    [[unlikely]] issue(result, Issue::expected_valid_expression);
    return;
  }
  }
}

void Parser::advance() noexcept { _lexer.next(_current); }

void Parser::expr_4(Token &result) noexcept {
  expr_3(result);
  while (true) {
    switch (_current.type) {
    case Token::Type::ADD: {
      if (result.type == Token::Type::NUM) {
        advance();
        Token number;
        expr_3(number);
        if (number.type == Token::Type::NUM) {
          result.number += number.number;
          break;
        } else {
          result.position = number.position;
          goto issue_expected_number;
        }
      } else {
        goto issue_expected_number;
      }
    }
    case Token::Type::SUB: {
      if (result.type == Token::Type::NUM) {
        advance();
        Token number;
        expr_3(number);
        if (number.type == Token::Type::NUM) {
          result.number -= number.number;
          break;
        } else {
          result.position = number.position;
          goto issue_expected_number;
        }
      } else {
        goto issue_expected_number;
      }
    }
    default: {
      return;
    }
    }
  }
issue_expected_number:
  [[unlikely]] issue(result, Issue::expected_number);
}

void Parser::expr_3(Token &result) noexcept {
  expr_2(result);
  while (true) {
    switch (_current.type) {
    case Token::Type::MUL: {
      if (result.type == Token::Type::NUM) {
        advance();
        Token number;
        expr_2(number);
        if (number.type == Token::Type::NUM) {
          result.number *= number.number;
          break;
        } else {
          result.position = number.position;
          goto issue_expected_number;
        }
      } else {
        goto issue_expected_number;
      }
    }
    case Token::Type::DIV: {
      if (result.type == Token::Type::NUM) {
        advance();
        Token number;
        expr_2(number);
        if (number.type == Token::Type::NUM) {
          result.number /= number.number;
          break;
        } else {
          result.position = number.position;
          goto issue_expected_number;
        }
      } else {
        goto issue_expected_number;
      }
    }
    default: {
      return;
    }
    }
  }
issue_expected_number:
  [[unlikely]] issue(result, Issue::expected_number);
}

void Parser::expr_2(Token &result) noexcept {
  ParamCount count = 0;
  std::array<Token, std::numeric_limits<ParamCount>::max()> tokens
      [[indeterminate]];
  do {
    expr_1(tokens[count]);
    if (_current.type == Token::Type::POW) {
      advance();
    } else {
      break;
    }
  } while (++count != static_cast<ParamCount>(tokens.size()));
  if (count == static_cast<ParamCount>(tokens.size())) [[unlikely]] {
    result.position = _current.position;
    issue(result, Issue::too_many_in_expression);
    return;
  }

  result = tokens[count];

  switch (result.type) {
  case Token::Type::NUM: {
    while (--count != static_cast<ParamCount>(-1)) {
      switch (tokens[count].type) {
      case Token::Type::NUM: {
        result.number = Identifiers::pow(tokens[count].number, result.number);
        break;
      }
      case Token::Type::ISSUE: {
        [[unlikely]] result = tokens[count];
        return;
      }
      default: {
        result.position = tokens[count].position;
        [[unlikely]] issue(result, Issue::expected_number);
        return;
      }
      }
    }
  case Token::Type::ISSUE: {
    [[unlikely]] return;
  }
  default: {
    [[unlikely]] issue(result, Issue::expected_valid_expression);
    return;
  }
  }
  }
}

void Parser::expr_1(Token &result) noexcept {
  switch (_current.type) {
  case Token::Type::SUB: {
    advance();
    expr_0(result);
    switch (result.type) {
    case Token::Type::NUM: {
      result.number = -result.number;
      return;
    }
    case Token::Type::ISSUE: {
      [[unlikely]] result = _current;
      return;
    }
    default: {
      [[unlikely]] issue(result, Issue::expected_number);
      return;
    }
    }
  }
  default: {
    expr_0(result);
    return;
  }
  }
}

void Parser::expr_0(Token &result) noexcept {
  switch (_current.type) {
  case Token::Type::LPAREN: {
    subexpression(result);
    return;
  }
  case Token::Type::NUM: {
    number(result);
    return;
  }
  case Token::Type::FUNCT: {
    function(result);
    return;
  }
  case Token::Type::CONST: {
    constant(result);
    return;
  }
  default: {
    result = _current;
    return;
  }
  }
}

void Parser::subexpression(Token &result) noexcept {
  advance();
  expr_4(result);
  if (_current.type == Token::Type::RPAREN) {
    advance();
  } else {
    result.position = _current.position;
    [[unlikely]] issue(result, Issue::expected_right_parenthesis);
  }
}

void Parser::constant(Token &result) noexcept {
  const auto &[caller, _] = _current.identifier->second;
  result.type = Token::Type::NUM;
  result.number = caller({});
  advance();
}

void Parser::number(Token &result) noexcept {
  result.type = Token::Type::NUM;
  result.number = _current.number;
  advance();
}

void Parser::function(Token &result) noexcept {
  result = _current;
  advance();
  if (_current.type == Token::Type::LPAREN) {
    advance();
    const auto &[caller, check] = result.identifier->second;
    ParamCount count = 0;
    std::array<Token, std::numeric_limits<ParamCount>::max()> parameters
        [[indeterminate]];

    do {
      expr_4(parameters[count]);
      ++count;
      switch (_current.type) {
      case Token::Type::RPAREN: {
        advance();
        if (check.params_count_is_valid(count)) {
          std::array<Value, std::numeric_limits<ParamCount>::max()> values
              [[indeterminate]];
          for (ParamCount i = 0; i != count; ++i) {
            if (parameters[i].type == Token::Type::NUM) {
              values[i] = parameters[i].number;
            } else {
              result.position = parameters[i].position;
              [[unlikely]] issue(result, Issue::expected_valid_expression);
              return;
            }
          }
          result.number = caller({values.begin(), values.begin() + count});
          result.type = Token::Type::NUM;
          return;
        } else {
          [[unlikely]] issue(result, Issue::incorrect_parameters_count);
          return;
        }
      }
      case Token::Type::COMA: {
        advance();
        continue;
      }
      default: {
        result.position = _current.position;
        [[unlikely]] issue(result, Issue::expected_right_parenthesis);
        return;
      }
      }
    } while (count != static_cast<ParamCount>(parameters.size()));

    [[unlikely]] issue(result, Issue::too_many_parameters);
    return;
  } else {
    result.position = _current.position;
    [[unlikely]] issue(result, Issue::expected_left_parenthesis);
    return;
  }
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
