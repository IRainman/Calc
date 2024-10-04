#pragma once
/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

namespace Identifiers
{
	/*
	 * Type using as a type for calculation. 
	 */
	using Value = long double;

	struct Fn
	{
		struct P
		{
			constexpr static auto unlim = std::numeric_limits<size_t>::max();
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
			[[nodiscard]] constexpr bool params_count_is_valid(const size_t count) const noexcept
			{
				return min <= count || count <= max;
			}
			const size_t min; //-V122
			const size_t max; //-V122
		} check;
		Value(*fn)(std::span<const Value>); //-V122
	};

	using map = std::unordered_map<std::string_view, const Fn>;
	/**
	 * Return the link to the global static collection of identifiers.
	 */
	[[nodiscard]] const map& get() noexcept;
};
