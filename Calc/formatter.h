#pragma once

#include <bit>
#include <charconv>
#include <format>
#include <limits>
#include <string>
#include "issue_manager.h"
/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

namespace Formatter
{
	/**
	 * Format output value of the expression returned.
	 */
	template<typename V>
	[[nodiscard]] constexpr auto format_output_value(const V v) noexcept
	{
		std::string s;
		s.resize(std::bit_ceil(static_cast<size_t>(std::numeric_limits<V>::max_digits10))); //-V201
		s.resize(std::to_chars(s.data(), s.data() + s.size(),
			v, std::chars_format::general, std::numeric_limits<V>::max_digits10).ptr - s.data());
		return s;
	}

	template <typename... Args>
	[[nodiscard]] constexpr auto format(const std::format_string<Args&&...> fmt, Args&&... args) noexcept {
		return std::format(fmt, args...);
	}

	[[nodiscard]] constexpr std::string format(const Issue::Severity severity) noexcept
	{
		switch (severity)
		{
			case Issue::Severity::INFO: [[unlikely]]
				return "Info";
			case Issue::Severity::WARN: [[unlikely]]
				return "Warning";
			case Issue::Severity::ERR: [[likely]]
				return "Error";
		}
		std::unreachable();
	}

	[[nodiscard]] constexpr std::string format(const Issue& issue) noexcept
	{
		return format(issue.severity) + " at pos " + std::to_string(issue.pos) + ": " + issue.text + "\r\n";
	}

	/**
	 * Create full report of expression processing.
	 */
	[[nodiscard]] std::string create_summary() noexcept;
};

