/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#include "pch.hpp"
#ifdef CALC_TESTS_ENABLED
#include <chrono>
#ifdef CALC_TEST_FASTFLOAT
#include <cfenv>
#endif
#include "formatter.hpp"
#include "issue_manager.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "tests.hpp"
#include "tests_equasions.hpp"

#ifdef CALC_TEST_BINARY_FUNCTIONS
[[nodiscard]] constexpr Value bin(const std::string_view x) noexcept {
  uint32_t bin_val;
  auto res = fast_float::from_chars(x.data(), x.data() + x.size(), bin_val, 2);
  if (res.ec == std::errc{}) [[likely]] {
    return bin_val;
  }

  [[unlikely]] return std::numeric_limits<Value>::quiet_NaN();
}

[[nodiscard]] constexpr Value hex(const std::string_view x) noexcept {
  uint32_t hex_val;
  auto res = fast_float::from_chars(x.data(), x.data() + x.size(), hex_val, 16);
  if (res.ec == std::errc{}) [[likely]] {
    return hex_val;
  }

  [[unlikely]] return std::numeric_limits<Value>::quiet_NaN();
}

constexpr std::string binary_and_hex_parsing() {
  std::string ret;
  {
    constexpr std::string_view data = "10101011110011011110111101101001";
    ret += fmt::format(FMT_COMPILE("bin {} -> {}\r\n"), data, bin(data));
  }
  {
    constexpr std::string_view data = "ABCDEF69";
    ret += fmt::format(FMT_COMPILE("hex {} -> {}\r\n"), data, hex(data));
  }
  return ret;
}
#endif

#ifdef CALC_TEST_FASTFLOAT

#define CASE_RETURN_NAME(x)                                                    \
  case x:                                                                      \
    return #x

constexpr std::string_view round_name(int const d) {
  switch (d) {
    CASE_RETURN_NAME(FE_UPWARD);
    CASE_RETURN_NAME(FE_DOWNWARD);
    CASE_RETURN_NAME(FE_TOWARDZERO);
    CASE_RETURN_NAME(FE_TONEAREST);
  default:
    std::unreachable();
  }
}

#undef CASE_RETURN_NAME
#endif

/*
static_assert(std::has_unique_object_representations_v<CalcWindowState>);
static_assert(std::is_trivially_copyable_v<CalcWindowState>);
static_assert(std::is_standard_layout_v<CalcWindowState>);
*/

std::string calc_tests() {
  std::string output;
#ifdef CALC_TESTS_DEV_ENABLED
  output.resize(85 * 1024);
  auto output_end = output.data();
  unsigned int failed = 0;
#endif

  const auto start = std::chrono::steady_clock::now();

#ifndef CALC_TESTS_DEV_ENABLED
  for (unsigned int i = 1'000'000; i-- != 0;)
#endif
  {
    for (const auto &t : tests) {
      Lexer l(t.first);
      Parser p(l);
      [[maybe_unused]] const auto value = p.parse();
      const auto has_errors = IssueManager::has_errors();

#ifdef CALC_TESTS_DEV_ENABLED // Development
      Formatter::Result buffer_value [[indeterminate]];
      Formatter::Result buffer_test [[indeterminate]];

      const std::string_view formated_value(
          buffer_value.data(), Formatter::format(value, buffer_value));

      const auto is_error = std::isnan(t.second) && has_errors;
      const auto is_nan = std::isnan(t.second) && std::isnan(value);
      const auto is_equal = t.second == value;
      const auto is_less_than_epsilon = Identifiers::compare(t.second, value);
      const auto is_normal = isnormal(value);
      const auto is_identical_output =
          formated_value ==
          std::string_view(buffer_test.data(),
                           Formatter::format(t.second, buffer_test));

      const auto passed = is_error || is_nan ||
                          /* is_equal || is_less_than_epsilon ||*/
                          is_identical_output;

      if (!passed) {
        ++failed;
      }

      Formatter::Summary buffer_summary [[indeterminate]];
      const std::string_view formated_summary(
          buffer_summary.data(), Formatter::create_summary(buffer_summary));

      IssueManager::clear();

      // clang-format off
      output_end = fmt::format_to(output_end, FMT_COMPILE("Test {}: {}\r\n"

          "is error = {}\r\n"
          "is nan = {}\r\n"
          "is equal = {}\r\n"
          "is less than epsilon = {}\r\n"
          "is normal = {}\r\n"
          "is identical output = {}\r\n"

          "return = {}\r\n"
          "expect = {}\r\n"

          "output = {}\r\n"
          "{}\r\n"),
                                           passed ? "OK" : "FAILED", t.first,
          is_error,
          is_nan,
          is_equal,
          is_less_than_epsilon,
          is_normal,
          is_identical_output,

          value,
          t.second,

 has_errors ? "" : formated_value,
!has_errors ? "" : formated_summary);
      // clang-format on
#else // Performance
      if (has_errors) {
        Formatter::Summary buffer_summary [[indeterminate]];
        [[maybe_unused]] const std::string_view formated_summary(
            buffer_summary.data(), Formatter::create_summary(buffer_summary));
        IssueManager::clear();
      } else {
        Formatter::Result buffer_value [[indeterminate]];
        [[maybe_unused]] const std::string_view formated_value(
            buffer_value.data(), Formatter::format(value, buffer_value));
      }
#endif
    }
  }
  const auto end = std::chrono::steady_clock::now();

#ifdef CALC_TESTS_DEV_ENABLED
  output.resize(output_end - output.data());
#endif

  // Don't use fmt here, because it isn't compile.
  output += std::format("Tests"
#ifdef CALC_TESTS_DEV_ENABLED
                        ":\r\n passed: {},\r\n failed: {}\r\n"
#ifdef CALC_TEST_FASTFLOAT
                        "fegetround() == {}\r\n"
#endif
#endif
                        " time is: {}.",
#ifdef CALC_TESTS_DEV_ENABLED
                        tests.size() - failed, failed,
#ifdef CALC_TEST_FASTFLOAT
                        round_name(std::fegetround()),
#endif
#endif
                        std::chrono::duration_cast<std::chrono::
#ifdef CALC_TESTS_DEV_ENABLED
                                                       microseconds
#else
                                                       milliseconds
#endif
                                                   >(end - start));

#ifdef CALC_TEST_BINARY_FUNCTIONS
  output += "\r\n\r\n" + binary_and_hex_parsing();
#endif
  output.shrink_to_fit();
  return output;
}

#endif
