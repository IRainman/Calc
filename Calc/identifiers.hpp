/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef IDENTIFIERS_HPP
#define IDENTIFIERS_HPP

namespace Identifiers {
struct Fn {
  [[no_unique_address]] Value (*const fn)(std::span<Value>);

  [[no_unique_address]] struct P {
    /**
     * Test is Fn is a constant and no needs arguments and simply returns a
     * constant value.
     */
    [[nodiscard]] constexpr bool is_constant() const noexcept {
      return !is_function();
    }

    /**
     * Test is Fn is a function that may needs arguments and before return do
     * some work.
     */
    [[nodiscard]] constexpr bool is_function() const noexcept {
      return this_is_function;
    }

    /**
     * Test interval of parameters count.
     */
    [[nodiscard]] constexpr bool
    params_count_is_valid(const ParamCount count) const noexcept {
      return min <= count && count <= max;
    }

    [[no_unique_address]] const bool this_is_function;
    [[no_unique_address]] const ParamCount min;
    [[no_unique_address]] const ParamCount max;
  } check;
};

/**
 * Return the link to the global static collection of identifiers.
 */
using map = const std::unordered_map<std::string_view, const Fn>;
[[nodiscard]] const map &get() noexcept;

[[nodiscard]] /*constexpr*/ Value pow(const Value x, const Value y) noexcept;

[[nodiscard]] /*constexpr*/ bool compare(const Value a, const Value b) noexcept;
}; // namespace Identifiers
#endif
