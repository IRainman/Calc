#pragma once

#include <bit>
#include <charconv>
#include <format>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>
#include "issue_manager.h"

/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

class Formatter
{
	public:
		/**
		 * Format output value of the expression returned.
		 */
		template<typename V>
		[[nodiscard]] constexpr static auto format(const V value, const bool is_constant = false) noexcept
		{
			static_assert(std::is_floating_point_v<V>);
			std::string out;
			out.resize(std::bit_ceil(static_cast<size_t>(std::numeric_limits<V>::max_digits10))); //-V201
			out.resize(std::to_chars(out.data(), out.data() + out.size(),
				value, std::chars_format::general, is_constant ? std::numeric_limits<V>::max_digits10 : std::numeric_limits<V>::digits10).ptr - out.data());
			return out;
		}

		template <typename... Args>
		[[nodiscard]] constexpr static auto format(const std::format_string<Args&&...> fmt, Args&&... args) noexcept {
			return std::format(fmt, args...);
		}

		[[nodiscard]] constexpr static std::string format(const Issue::Severity severity) noexcept
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

		[[nodiscard]] constexpr static std::string format(const Issue& issue) noexcept
		{
			return format(issue.severity) + " at pos " + std::to_string(issue.pos) + ": " + issue.text + "\r\n";
		}

		/**
		 * Create full report of expression processing.
		 */
		[[nodiscard]] static std::string create_summary() noexcept;
};

