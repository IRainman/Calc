/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#include "pch.hpp"
#ifdef CALC_TESTS_ENABLED
#include "formatter.hpp"
#include "issue_manager.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "tests.hpp"
#include "tests_equasions.hpp"
#include <cfenv>

[[nodiscard]] constexpr static Value bin(const std::string_view x) noexcept {
  UInteger bin_val;
  auto res = fast_float::from_chars(x.data(), x.data() + x.size(), bin_val, 2);
  if (res.ec == std::errc{}) [[likely]] {
    return static_cast<Value>(bin_val);
  }

  [[unlikely]] return std::numeric_limits<Value>::quiet_NaN();
}

[[nodiscard]] constexpr static Value hex(const std::string_view x) noexcept {
  UInteger hex_val;
  auto res = fast_float::from_chars(x.data(), x.data() + x.size(), hex_val, 16);
  if (res.ec == std::errc{}) [[likely]] {
    return static_cast<Value>(hex_val);
  }

  [[unlikely]] return std::numeric_limits<Value>::quiet_NaN();
}

constexpr static auto binary_and_hex_parsing(char *ret) {
  {
    constexpr std::string_view data = "10101011110011011110111101101001";
    ret = fmt::format_to(ret, FMT_COMPILE("bin {} -> {}\n"), data, bin(data));
  }
  {
    constexpr std::string_view data = "ABCDEF69";
    ret = fmt::format_to(ret, FMT_COMPILE("hex {} -> {}\n"), data, hex(data));
  }
  return ret;
}

#define CASE_RETURN_NAME(x)                                                    \
  case x:                                                                      \
    return #x

constexpr std::string_view round_name(int const d) {
  switch (d) {
    CASE_RETURN_NAME(FE_TONEAREST);
    CASE_RETURN_NAME(FE_DOWNWARD);
    CASE_RETURN_NAME(FE_UPWARD);
    CASE_RETURN_NAME(FE_TOWARDZERO);
  default:
    std::unreachable();
  }
}

#undef CASE_RETURN_NAME

/*
static_assert(std::has_unique_object_representations_v<CalcWindowState>);
static_assert(std::is_trivially_copyable_v<CalcWindowState>);
static_assert(std::is_standard_layout_v<CalcWindowState>);
*/

std::string calc_tests() {
  std::string output;
  output.resize(
#ifdef CALC_TESTS_DEV_ENABLED // Development
      128 * 1024
#else // Performance
      std::hardware_destructive_interference_size
#endif
  );

  auto output_end = output.data();

  const auto start = std::chrono::steady_clock::now();

#ifdef CALC_TESTS_DEV_ENABLED // Development
  unsigned int failed = 0;
#else // Performance
  constexpr unsigned int count = 100'000;
  for (unsigned int i = count; --i != 0;)
#endif
  {
    for (const auto &t : tests) {
      Lexer l(t.first);
      Parser p(l);
      const auto token = p.result();
      const auto is_issue = token.type == Token::Type::ISSUE;

#ifdef CALC_TESTS_DEV_ENABLED // Development
      Result buffer_value [[indeterminate]];
      Result buffer_test [[indeterminate]];

      const std::string_view formated_value(buffer_value.data(),
                                            value(token.number, buffer_value));
      const std::string_view formated_test(buffer_test.data(),
                                           value(t.second, buffer_test));

      const auto is_error = std::isnan(t.second) && is_issue;
      const auto is_nan = std::isnan(t.second) && std::isnan(token.number);
      const auto is_equal = t.second == token.number;
      const auto is_less_than_epsilon =
          Identifiers::compare(t.second, token.number);
      const auto is_normal = std::isnormal(token.number);
      const auto is_identical_output = formated_value == formated_test;

      const auto passed = is_error || is_nan || is_identical_output;

      if (!passed) {
        ++failed;
      }

      Result buffer [[indeterminate]];
      const std::string_view formated_report(buffer.data(), report(buffer));

      // clang-format off
      output_end = fmt::format_to(output_end, FMT_COMPILE("Test {}: {}\n"
                                                          "is error = {}\n"
                                                          "is nan = {}\n"
                                                          "is equal = {}\n"
                                                          "is less than epsilon = {}\n"
                                                          "is normal = {}\n"
                                                          "is identical output = {}\n"
                                                          "return = {}\n"
                                                          "expect = {}\n"
                                                          "output = {}\n"
                                                          "{}\n"),

                                                 passed ? "OK" : "FAILED", t.first,
                                                          is_error,
                                                          is_nan,
                                                          is_equal,
                                                          is_less_than_epsilon,
                                                          is_normal,
                                                          is_identical_output,
                                                          token.number,
                                                          t.second,
                                               is_issue ? "" : formated_value,
                                              !is_issue ? "" : formated_report
);
      // clang-format on
#else // Performance
      Result buffer [[indeterminate]];
      [[maybe_unused]] const std::string_view formated(buffer.data(),
                                                       result(token, buffer));
#endif
    }
  }
  const auto end = std::chrono::steady_clock::now();

#ifdef CALC_TESTS_DEV_ENABLED // Development
  output_end = binary_and_hex_parsing(output_end);

  output_end =
      fmt::format_to(output_end,
                     FMT_COMPILE("Tests:\n"
                                 " fegetround() == {}\n"
                                 " passed: {},\n failed: {}\n"
                                 " time is: {}ns per case."),
                     round_name(std::fegetround()),
                     tests.size() - static_cast<size_t>(failed), failed,
                     std::chrono::duration_cast<std::chrono::nanoseconds>(
                         (end - start) / tests.size())
                         .count());
#else // Performance
  output_end =
      fmt::format_to(output_end, FMT_COMPILE("Performance tests time is: {}ns per case."),
                     std::chrono::duration_cast<std::chrono::nanoseconds>(
                         (end - start) / (tests.size() * count))
                         .count());
#endif

  output.resize(output_end - output.data());

  return output;
}
#endif
