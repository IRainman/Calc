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
  case Token::Type::RESULT:
    if (result.type == Token::Type::NUM) {
      return;
    } else [[unlikely]] {
      issue(result, _lexer.position(), Issue::expected_number);
      return;
    }
  case Token::Type::ISSUE:
    [[unlikely]] result = _current;
    return;
  default:
    [[unlikely]] issue(result, _lexer.position(), Issue::extraneous_input);
    return;
  }
}

void Parser::advance() noexcept { _lexer.next(_current); }

void Parser::expr_4(Token &result) noexcept {
  expr_3(result);
  while (true) {
    switch (_current.type) {
    case Token::Type::ADD:
      if (result.type == Token::Type::NUM) {
        advance();
        Token number;
        expr_3(number);
        if (number.type == Token::Type::NUM) {
          result.number += number.number;
          break;
        } else [[unlikely]] {
          issue(result, _lexer.position(), Issue::expected_number);
          return;
        }
      } else [[unlikely]] {
        issue(result, _lexer.position(), Issue::expected_number);
        return;
      }
    case Token::Type::SUB:
      if (result.type == Token::Type::NUM) {
        advance();
        Token number;
        expr_3(number);
        if (number.type == Token::Type::NUM) {
          result.number -= number.number;
          break;
        } else [[unlikely]] {
          issue(result, _lexer.position(), Issue::expected_number);
          return;
        }
      } else [[unlikely]] {
        issue(result, _lexer.position(), Issue::expected_number);
        return;
      }
    default:
      return;
    }
  }
}

void Parser::expr_3(Token &result) noexcept {
  expr_2(result);
  while (true) {
    switch (_current.type) {
    case Token::Type::MUL:
      if (result.type == Token::Type::NUM) {
        advance();
        Token number;
        expr_2(number);
        if (number.type == Token::Type::NUM) {
          result.number *= number.number;
          break;
        } else [[unlikely]] {
          issue(result, _lexer.position(), Issue::expected_number);
          return;
        }
      } else [[unlikely]] {
        issue(result, _lexer.position(), Issue::expected_number);
        return;
      }
    case Token::Type::DIV:
      if (result.type == Token::Type::NUM) {
        advance();
        Token number;
        expr_2(number);
        if (number.type == Token::Type::NUM) {
          result.number /= number.number;
          break;
        } else [[unlikely]] {
          issue(result, _lexer.position(), Issue::expected_number);
          return;
        }
      } else [[unlikely]] {
        issue(result, _lexer.position(), Issue::expected_number);
        return;
      }
    default:
      return;
    }
  }
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
    issue(result, _lexer.position(), Issue::too_many_in_expression);
    return;
  }

  result = tokens[count];

  if (result.type == Token::Type::NUM) {
    while (--count != static_cast<ParamCount>(-1)) {
      if (tokens[count].type == Token::Type::NUM) {
        result.number = Identifiers::pow(tokens[count].number, result.number);
      } else [[unlikely]] {
        issue(result, _lexer.position(), Issue::expected_number);
        return;
      }
    }
  } else [[unlikely]] {
    issue(result, _lexer.position(), Issue::expected_number);
    return;
  }
}

void Parser::expr_1(Token &result) noexcept {
  switch (_current.type) {
  case Token::Type::SUB: {
    advance();
    expr_0(result);
    if (result.type == Token::Type::NUM) {
      result.number = -result.number;
      return;
    } else [[unlikely]] {
      issue(result, _lexer.position(), Issue::expected_number);
      return;
    }
  }
  default:
    expr_0(result);
    return;
  }
}

void Parser::expr_0(Token &result) noexcept {
  switch (_current.type) {
  case Token::Type::LPAREN:
    subexpression(result);
    return;
  case Token::Type::NUM:
    number(result);
    return;
  case Token::Type::FUNCT:
    function(result);
    return;
  case Token::Type::CONST:
    constant(result);
    return;
  default:
    result = _current;
    return;
  }
}

void Parser::subexpression(Token &result) noexcept {
  // auto subexpression_start_pos = _lexer.position();
  advance();
  expr_4(result);
  if (_current.type == Token::Type::RPAREN) {
    advance();
  } else [[unlikely]] {
    issue(result, _lexer.position(), Issue::expected_parenthesis);
    return;
  }
}

void Parser::constant(Token &result) noexcept {
  // auto constant_start_pos = _lexer.position();
  const auto &[caller, _] = _current.identifier->second;
  result.type = Token::Type::NUM;
  result.number = caller({});
  advance();
}

void Parser::number(Token &result) noexcept {
  // auto number_start_pos = _lexer.position();
  result.type = Token::Type::NUM;
  result.number = _current.number;
  advance();
}

void Parser::function(Token &result) noexcept {
  const auto function_start_pos = _lexer.position();
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
            } else [[unlikely]] {
              issue(result, function_start_pos, Issue::expected_number);
              return;
            }
          }
          result.type = Token::Type::NUM;
          result.number = caller({values.begin(), values.begin() + count});
          return;
        } else [[unlikely]] {
          issue(result, function_start_pos - result.identifier->first.size(),
                Issue::incorrect_parameters_count);
          return;
        }
      }
      case Token::Type::COMA: {
        advance();
        continue;
      }
      default:
        [[unlikely]] {
          issue(result, _lexer.position(), Issue::expected_parenthesis);
          return;
        }
      }
    } while (count != static_cast<ParamCount>(parameters.size()));

    [[unlikely]] issue(result, _lexer.position(), Issue::too_many_parameters);
    return;
  } else [[unlikely]] {
    issue(result, _lexer.position(), Issue::expected_parenthesis);
    return;
  }
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
