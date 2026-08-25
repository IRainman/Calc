/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#include "pch.hpp"

#include "formatter.hpp"
#include "issue_manager.hpp"

#ifdef CALC_SUPPORT_FRACTIONAL_OUTPUT
#include "identifiers.hpp"

// Function to convert decimal to fraction
std::pair<Integer, Integer> decimalToFraction(Value number) {
  if (std::isnan(number) || std::isinf(number))
    return {0, 1};

  // Fetch integral value of the decimal
  Value intVal = std::floor(number);

  // Fetch fractional part of the decimal
  Value fVal = number - intVal;

  // Consider precision value to
  // convert fractional part to
  // integral equivalent
  constexpr Integer pVal = 1e15;

  // Calculate GCD of integral
  // equivalent of fractional
  // part and precision value
  Integer gcdVal = Identifiers::gcd(std::round(fVal * pVal), pVal);

  // Calculate num and deno
  Integer num = std::round(fVal * pVal) / gcdVal;
  Integer deno = pVal / gcdVal;

  if (std::abs(num) > huge_value_precision || deno == 1 ||
      deno > huge_value_precision)
    return {0, 1};

  // Return the fraction
  return std::make_pair((intVal * deno) + num, deno);
}
#endif

char *Formatter::format(Value value, Result &ret) noexcept {
  char *end;
  // https://www.exploringbinary.com/decimal-precision-of-binary-floating-point-numbers/
  if (std::isnormal(value)) {
    end = zmij::detail::write_general(ret.data(), value, output_precision);
  } else {
    end = zmij::detail::write(ret.data(), value);
  }
  return end;
}

#ifndef CALC_USE_ERROR_TOKEN
char *Formatter::create_summary(Summary &ret) noexcept {
  auto end = ret.data();
  for (const auto &error : IssueManager::_errors) {
    end = fmt::format_to(end, FMT_COMPILE("{}: {}\r\n"), error.pos, error.text);
  }
  IssueManager::clear();
  return end;
}
#endif
