/*
 * Copyright 2023 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#pragma once

// TODO use std::format
template<typename S, typename V>
inline constexpr S& format_output_value(S& p_str, const V p_val)
{
	// TODO concepts
	p_str.resize(32); // TODO fix magic number... with what?
	p_str.resize(std::to_chars(p_str.data(), p_str.data() + p_str.size(), p_val, std::chars_format::general, std::numeric_limits<V>::digits10 - 1).ptr - p_str.data());
	return p_str;
}

template <typename... Args>
[[nodiscard]] std::string test_print(std::format_string<Args&&...> fmt, Args&&... args) {
	return std::format(fmt, args...);
}

namespace Formatter
{
	/**
	 * Return full report of expression processing and clear the issue manager queue.
	 */
	[[nodiscard]] std::string create_summary();
};

