/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#include "pch.hpp"

#include "formatter.hpp"
#include "issue_manager.hpp"

#ifdef CALC_SUPPORT_FRACTIONAL_OUTPUT
#include "identifiers.hpp"

// Function to convert decimal to fraction
constexpr static std::pair<Integer, Integer> decimalToFraction(Value number) {
  if (std::isnan(number) || std::isinf(number))
    return {0, 1};

  // Fetch integral value of the decimal
  Value intVal = std::floor(number);

  // Fetch fractional part of the decimal
  Value fVal = number - intVal;

  // Consider precision value to
  // convert fractional part to
  // integral equivalent
  constexpr Integer value = 1000000000000000;
  constexpr Integer limit = 100000000000000;

  // Calculate GCD of integral
  // equivalent of fractional
  // part and precision value
  Integer gcdVal =
      Identifiers::gcd(static_cast<Integer>(std::round(fVal * value)), value);

  // Calculate num and deno
  Integer num = static_cast<Integer>(std::round(fVal * value)) / gcdVal;
  Integer deno = value / gcdVal;

  if (std::abs(num) > limit || deno == 1 || deno > limit)
    return {0, 1};

  // Return the fraction
  return std::make_pair((static_cast<Integer>(intVal) * deno) + num, deno);
}
#endif

/**
 * Format Value.
 * @return the end of the formated text.
 */
char *value(Value value, Result &ret) noexcept {
  auto end = ret.data();
  // https://www.exploringbinary.com/decimal-precision-of-binary-floating-point-numbers/
  if (std::isnormal(value)) {
    end = zmij::detail::write_general(end, value, output_precision);
  } else {
    end = zmij::detail::write(end, value);
  }
  return end;
}

/**
 * Format Token.
 * @return the end of the formated text.
 */
char *result(Token token, Result &ret) noexcept {
  auto end = ret.data();
  if (token.type == Token::Type::ISSUE) {
    end = report(ret);
  } else {
    end = value(token.number, ret);
  }
  return end;
}
