#pragma once
/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include <limits>
#include <span>
#include <unordered_map>
#include <string_view>
#include "token.h"

namespace Identifiers
{
	using Value = Token::Value;
	using ParamCount = Token::ParamCount;
	struct Fn
	{
		struct P
		{
			constexpr static auto unlim = std::numeric_limits<ParamCount>::max();
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
			const ParamCount min;
			const ParamCount max;
		} check;
		Value(*fn)(std::span<const Value>); //-V122
	};

	using map = std::unordered_map<std::string_view, const Fn>;
	/**
	 * Return the link to the global static collection of identifiers.
	 */
	[[nodiscard]] const map& get() noexcept;
};
