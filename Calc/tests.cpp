/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#include "pch.hpp"
#ifdef CALC_TESTS_ENABLED
#include <chrono>
#ifdef CALC_TEST_FASTFLOAT
#include <cfenv>
#endif
#include "lexer.hpp"
#include "parser.hpp"
#include "issue_manager.hpp"
#include "formatter.hpp"
#include "tests.hpp"
#include "tests_equasions.hpp"

#ifdef CALC_TEST_FASTFLOAT
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

constexpr std::string test_fast_float_parsing() {
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
constexpr std::string_view round_name(int const d) {
  switch (d) {
  case FE_UPWARD:
    return "FE_UPWARD";
  case FE_DOWNWARD:
    return "FE_DOWNWARD";
  case FE_TOWARDZERO:
    return "FE_TOWARDZERO";
  case FE_TONEAREST:
    return "FE_TONEAREST";
  default:
    return "UNKNOWN";
  }
}
#endif

/*
static_assert(std::has_unique_object_representations_v<CalcWindowState>);
static_assert(std::is_trivially_copyable_v<CalcWindowState>);
static_assert(std::is_standard_layout_v<CalcWindowState>);
*/

std::string calc_tests() {
  std::string output;
#ifdef CALC_TESTS_DEV_ENABLED
  output.resize(64 * 1024);
  auto output_end = output.data();
  unsigned int failed = 0;
  unsigned int doubtful_but_okey = 0;
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

#ifdef CALC_TESTS_DEV_ENABLED
      Formatter::Result buffer_value [[indeterminate]];
      Formatter::Result buffer_test [[indeterminate]];
      const std::string_view formated_value(
          buffer_value.data(), Formatter::format(value, buffer_value));
      const auto exactly =
          (has_errors && std::isnan(t.second)) ||
          (std::isnan(value) && std::isnan(t.second)) ||
          (formated_value ==
           std::string_view(buffer_test.data(),
                            Formatter::format(t.second, buffer_test)));

      const auto less_than_epsilon = Identifiers::compare(value, t.second);

      if (!exactly) {
        ++failed;
      }
      if (!less_than_epsilon) {
        ++doubtful_but_okey;
      }

      Formatter::Summary buffer_summary [[indeterminate]];
      const std::string_view formated_summary(
          buffer_summary.data(), Formatter::create_summary(buffer_summary));
      IssueManager::clear();

      output_end = fmt::format_to(output_end,
                                  FMT_COMPILE("Test {}: {}\r\n"
                                              "return = {}\r\n"
                                              "expect = {}\r\n"
                                              "output = {}\r\n"
                                              "{}\r\n"),
                                  exactly ? "OK" : "FAILED", t.first, value,
                                  t.second, has_errors ? "" : formated_value,
                                  has_errors ? formated_summary : "");

#else

      if (has_errors) {
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
  output += std::format(
      "Tests"
#ifdef CALC_TESTS_DEV_ENABLED
      ":\r\n exactly: {},\r\n less than epsilon: {},\r\n failed: {}\r\n"
#ifdef CALC_TEST_FASTFLOAT
      "fegetround() == {}\r\n"
#endif
#endif
      " time is: {}.",
#ifdef CALC_TESTS_DEV_ENABLED
      tests.size() - failed - doubtful_but_okey, doubtful_but_okey, failed,
#ifdef CALC_TEST_FASTFLOAT
      round_name(fegetround()),
#endif
#endif
      std::chrono::duration_cast<std::chrono::
#ifdef CALC_TESTS_DEV_ENABLED
                                     microseconds
#else
                                     milliseconds
#endif
                                 >(end - start));

#ifdef CALC_TEST_FASTFLOAT
  output += "\r\n\r\n" + test_fast_float_parsing();
#endif
  output.shrink_to_fit();
  return output;
}

#endif
