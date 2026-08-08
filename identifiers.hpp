#pragma once

/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

namespace Identifiers {
struct Fn {
  struct P {
    /**
     * Test is Fn is a constant and no needs arguments.
     */
    [[nodiscard]] constexpr bool is_constant() const noexcept {
      return min == 0 && max == 0;
    }

    /**
     * Test is Fn is a function and needs arguments.
     */
    [[nodiscard]] constexpr bool is_function() const noexcept {
      return !is_constant();
    }

    /**
     * Test interval of parameters count.
     */
    [[nodiscard]] constexpr bool
    params_count_is_valid(const ParamCount count) const noexcept {
      return min <= count && count <= max;
    }

    [[no_unique_address]] const ParamCount min;
    [[no_unique_address]] const ParamCount max;
  } check;

  Value (*const fn)(std::span<Value>); //-V122
};

/**
 * Return the link to the global static collection of identifiers.
 */
using map = const std::unordered_map<std::string_view, const Fn>;
[[nodiscard]] const map &get() noexcept;

[[nodiscard]] /*constexpr*/ Value pow(const Value x, const Value y) noexcept;

[[nodiscard]] constexpr Value degrees_to_radians(const Value x) noexcept;

[[nodiscard]] constexpr Value radians_to_degrees(const Value x) noexcept;

[[nodiscard]] /*constexpr*/ bool compare(const Value a, const Value b) noexcept;
}; // namespace Identifiers
