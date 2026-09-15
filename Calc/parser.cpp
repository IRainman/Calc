/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#include "pch.hpp"

#include "issue_manager.hpp"
#include "parser.hpp"

namespace {
const auto &ids = Identifiers::get();
};

// clang-format off
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4061) // enumerator in switch of enum not explicitly handled
#endif
// clang-format on

[[nodiscard]] Token Parser::result() noexcept {
  advance();
  auto result = expr_4();
  switch (_current.type) {
  case Token::Type::RESULT:
    if (result.type == Token::Type::NUM) [[likely]] {
      return result;
    } else [[unlikely]] {
      return issue(_current, _lexer.position(), Issue::expected_number);
    }
  case Token::Type::ISSUE:
    [[unlikely]] return _current;
  }
  return issue(_current, _lexer.position(), Issue::unexpected);
}

void Parser::advance() noexcept { _lexer.next(_current); }

[[nodiscard]] Token Parser::expr_4() noexcept {
  auto result = expr_3();
  while (true) {
    switch (_current.type) {
    case Token::Type::ADD:
      if (result.type == Token::Type::NUM) [[likely]] {
        advance();
        const auto number = expr_3();
        if (number.type == Token::Type::NUM) [[likely]] {
          result.number += number.number;
          break;
        } else [[unlikely]] {
          return issue(_current, _lexer.position(), Issue::expected_number);
        }
      } else [[unlikely]] {
        return issue(_current, _lexer.position(), Issue::expected_number);
      }
    case Token::Type::SUB:
      if (result.type == Token::Type::NUM) [[likely]] {
        advance();
        const auto number = expr_3();
        if (number.type == Token::Type::NUM) [[likely]] {
          result.number -= number.number;
          break;
        } else [[unlikely]] {
          return issue(_current, _lexer.position(), Issue::expected_number);
        }
      } else [[unlikely]] {
        return issue(_current, _lexer.position(), Issue::expected_number);
      }
    default:
      return result;
    }
  }
}

[[nodiscard]] Token Parser::expr_3() noexcept {
  auto result = expr_2();
  while (true) {
    switch (_current.type) {
    case Token::Type::MUL:
      if (result.type == Token::Type::NUM) [[likely]] {
        advance();
        const auto number = expr_2();
        if (number.type == Token::Type::NUM) [[likely]] {
          result.number *= number.number;
          break;
        } else [[unlikely]] {
          return issue(_current, _lexer.position(), Issue::expected_number);
        }
      } else [[unlikely]] {
        return issue(_current, _lexer.position(), Issue::expected_number);
      }
    case Token::Type::DIV:
      if (result.type == Token::Type::NUM) [[likely]] {
        advance();
        const auto number = expr_2();
        if (number.type == Token::Type::NUM) [[likely]] {
          result.number /= number.number;
          break;
        } else [[unlikely]] {
          return issue(_current, _lexer.position(), Issue::expected_number);
        }
      } else [[unlikely]] {
        return issue(_current, _lexer.position(), Issue::expected_number);
      }
    default:
      return result;
    }
  }
}

[[nodiscard]] Token Parser::expr_2() noexcept {
  ParamCount count = 0;
  std::array<Token, std::numeric_limits<ParamCount>::max()> tokens
      [[indeterminate]];
  do {
    tokens[count] = expr_1();
    if (_current.type == Token::Type::POW) {
      advance();
    } else {
      break;
    }
  } while (++count != static_cast<ParamCount>(tokens.size()));

  if (count == static_cast<ParamCount>(tokens.size())) [[unlikely]] {
    return issue(_current, _lexer.position(), Issue::too_many_in_expression);
  }

  auto &result = tokens[count];
  if (--count != static_cast<ParamCount>(-1)) {
    if (result.type == Token::Type::NUM) [[likely]] {
      do {
        if (tokens[count].type == Token::Type::NUM) [[likely]] {
          result.number = Identifiers::pow(tokens[count].number, result.number);
        } else [[unlikely]] {
          return issue(_current, _lexer.position(), Issue::expected_number);
        }
      } while (--count != static_cast<ParamCount>(-1));
    } else [[unlikely]] {
      return issue(_current, _lexer.position(), Issue::expected_number);
    }
  }

  return result;
}

[[nodiscard]] Token Parser::expr_1() noexcept {
  switch (_current.type) {
  case Token::Type::SUB: {
    advance();
    auto result = expr_0();
    if (result.type == Token::Type::NUM) [[likely]] {
      result.number = -result.number;
      return result;
    } else [[unlikely]] {
      return issue(_current, _lexer.position(), Issue::expected_number);
    }
  }
  default:
    return expr_0();
  }
}

[[nodiscard]] Token Parser::expr_0() noexcept {
  switch (_current.type) {
  case Token::Type::LPAREN:
    return subexpression();

  case Token::Type::NUM:
    return number();

  case Token::Type::FUNCT:
    return function();

  case Token::Type::CONST:
    return constant();

  default:
    return _current;
  }
}

[[nodiscard]] Token Parser::subexpression() noexcept {
  // auto subexpression_start_pos = _lexer.position();
  advance();
  const auto result = expr_4();
  if (_current.type == Token::Type::RPAREN) [[likely]] {
    advance();
    return result;
  } else {
    return issue(_current, _lexer.position(), Issue::expected_right_parenthesis);
  }
}

[[nodiscard]] Token Parser::constant() noexcept {
  // auto constant_start_pos = _lexer.position();
  auto result = _current;
  advance();
  const auto &[caller, _] = result.identifier->second;
  result.type = Token::Type::NUM;
  result.number = caller({});
  return result;
}

[[nodiscard]] Token Parser::number() noexcept {
  // auto number_start_pos = _lexer.position();
  auto result = _current;
  advance();
  return result;
}

[[nodiscard]] Token Parser::function() noexcept {
  auto function_start_pos = _lexer.position();
  auto result = _current;
  advance();
  if (_current.type == Token::Type::LPAREN) [[likely]] {
    advance();
    const auto &[caller, check] = result.identifier->second;
    ParamCount count = 0;
    std::array<Token, std::numeric_limits<ParamCount>::max()> parameters
        [[indeterminate]];

    do {
      parameters[count] = expr_4();
      ++count;
      switch (_current.type) {
      case Token::Type::RPAREN:
        [[likely]] {
          advance();
          if (check.is_function() && check.params_count_is_valid(count))
              [[likely]] {
            std::array<Value, std::numeric_limits<ParamCount>::max()> values
                [[indeterminate]];
            for (ParamCount i = 0; i != count; ++i) {
              if (parameters[i].type == Token::Type::NUM) [[likely]] {
                values[i] = parameters[i].number;
              } else [[unlikely]] {
                return issue(result, function_start_pos,
                             Issue::expected_number);
              }
            }
            result.type = Token::Type::NUM;
            result.number = caller({values.begin(), count});
            return result;
          } else [[unlikely]] {
            function_start_pos -= result.identifier->first.size();
            return issue(result, function_start_pos,
                         Issue::incorrect_parameters_count);
          }
        }
      case Token::Type::COMA:
        [[likely]] {
          advance();
          continue;
        }
      default:
        [[unlikely]] {
          return issue(_current, _lexer.position(),
                       Issue::expected_right_parenthesis_or_comma);
        }
      }
    } while (count != static_cast<ParamCount>(parameters.size()));

    [[unlikely]] return issue(_current, _lexer.position(),
                              Issue::too_many_parameters);
  } else [[unlikely]] {
    return issue(_current, _lexer.position(), Issue::expected_left_parenthesis);
  }
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
