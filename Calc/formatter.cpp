/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#include "pch.hpp"

#include "formatter.hpp"
#include "issue_manager.hpp"

#ifdef CALC_SUPPORT_FRACTIONAL_OUTPUT
// Recursive function to
// return GCD of a and b
int64_t gcd(int64_t a, int64_t b) {
  if (a == 0)
    return b;
  else if (b == 0)
    return a;
  if (a < b)
    return gcd(a, b % a);
  else
    return gcd(b, a % b);
}

// Function to convert decimal to fraction
std::pair<int64_t, int64_t> decimalToFraction(Value number) {
  if (std::isnan(number) || std::isinf(number))
    return {0, 1};

  // Fetch integral value of the decimal
  Value intVal = std::floor(number);

  // Fetch fractional part of the decimal
  Value fVal = number - intVal;

  // Consider precision value to
  // convert fractional part to
  // integral equivalent
  constexpr int64_t pVal = 1e15;

  // Calculate GCD of integral
  // equivalent of fractional
  // part and precision value
  int64_t gcdVal = gcd(std::round(fVal * pVal), pVal);

  // Calculate num and deno
  int64_t num = std::round(fVal * pVal) / gcdVal;
  int64_t deno = pVal / gcdVal;

  if (std::abs(num) > 1e14 || deno == 1 || deno > 1e14)
    return {0, 1};

  // Return the fraction
  return std::make_pair((intVal * deno) + num, deno);
}
#endif

char *Formatter::format(Value value, Result &ret) noexcept { //-V2009
  char *end;
  // https://www.exploringbinary.com/decimal-precision-of-binary-floating-point-numbers/
  if (isnormal(value)) {
    end = zmij::detail::write_general(value, output_precision, ret.data());
  } else {
    end = zmij::detail::write(value, ret.data());
  }
  return end;
}

#ifndef CALC_USE_ERROR_TOKEN
char *Formatter::create_summary(Summary &ret) noexcept { //-V2009
  auto end = ret.data();
  for (const auto &error : IssueManager::_errors) {
    end = fmt::format_to(end, FMT_COMPILE("{}: {}\r\n"), error.pos, error.text);
  }
  return end;
}
#endif
