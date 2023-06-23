#pragma once
/*
 * Copyright 2023 Solomina Elle Leonovna, a.rainman on gmail point com
 */

namespace Formatter
{
	/**
	 * Format output value of the expression returned.
	 */
	template<typename V>
	[[nodiscard]] auto format_output_value(V v)
	{
		// TODO concepts
		std::string s; s.resize(32 /* std::numeric_limits<V>max_digits10 value round up to the nearest power of two */); // TODO fix magic number.
		s.resize(std::to_chars(s.data(), s.data() + s.size(), v, std::chars_format::general, std::numeric_limits<V>::digits10 - 1).ptr - s.data());
		return s;
	}

	template <typename... Args>
	[[nodiscard]] auto format(const std::format_string<Args&&...> fmt, Args&&... args) {
		return std::format(fmt, args...);
	}

	/**
	 * Create full report of expression processing.
	 */
	[[nodiscard]] std::string create_summary();
};

