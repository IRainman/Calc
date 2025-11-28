#pragma once
/*
 * Copyright 2023-2025 Solomina Elle, a.rainman on gmail point com
 */

namespace Identifiers
{
	struct Fn
	{
		struct P
		{
			/**
			 * Test is Fn is a constant and no needs arguments.
			 */
			[[nodiscard]] constexpr bool is_constant() const noexcept
			{
				return min == 0 && max == 0;
			}
			/**
			 * Test is Fn is a function and needs arguments.
			 */
			[[nodiscard]] constexpr bool is_function() const noexcept
			{
				return !is_constant();
			}
			/**
			 * Test interval of parameters count.
			 */
			[[nodiscard]] constexpr bool params_count_is_valid(const ParamCount count) const noexcept
			{
				return min <= count && count <= max;
			}
			[[no_unique_address]] const ParamCount min [[indeterminate]];
			[[no_unique_address]] const ParamCount max [[indeterminate]];
		} check;
		Value(*fn)(std::span<Value>); //-V122
	};

	using map = std::unordered_map<std::string_view, const Fn>;
	/**
	 * Return the link to the global static collection of identifiers.
	 */
	[[nodiscard]] const map& get() noexcept;

	[[nodiscard]] /*constexpr*/ Value pow(Value x, Value y) noexcept;

	[[nodiscard]] constexpr Value rad(Value x) noexcept;

	[[nodiscard]] constexpr Value deg(Value x) noexcept;

	[[nodiscard]] /*constexpr*/ bool compare(const Value a, const Value b) noexcept;
};
