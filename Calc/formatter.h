#pragma once

#include <bit>
#include <charconv>
#include <format>
#include <limits>
#include <string>
#include "token.h"
#include "issue_manager.h"

/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

class Formatter
{
	using Value = Token::Value;
	public:
		/**
		 * Format output value of the expression returned.
		 */
		[[nodiscard]] static std::string format(Value value) noexcept
		{
			std::string out;
			out.resize(std::bit_ceil(static_cast<size_t>(std::numeric_limits<Value>::max_digits10))); //-V201
			out.resize(std::to_chars(out.data(), out.data() + out.size(), value, std::chars_format::general, std::numeric_limits<Value>::max_digits10).ptr - out.data());
			return out;
		}

		template <typename... Args>
		[[nodiscard]] constexpr static inline auto format(const std::format_string<Args&&...> fmt, Args&&... args) noexcept {
			return std::format(fmt, args...);
		}

#ifdef ISSUE_MANAGER_HAVE_SEVERITY
		[[nodiscard]] constexpr static inline std::string format(const Issue::Severity severity) noexcept
		{
			switch (severity)
			{
				case Issue::Severity::INFO:
					return "Info";
				case Issue::Severity::WARN:
					return "Warning";
				case Issue::Severity::ERR:
					return "Error";
			}
			std::unreachable();
		}
#endif

		[[nodiscard]] constexpr static inline std::string format(const Issue& issue) noexcept
		{
			return 
#ifdef ISSUE_MANAGER_HAVE_SEVERITY
				format(issue.severity) +
#else
				"Error"
#endif
				" at pos " + std::to_string(issue.pos) + ": " + issue.text + "\r\n";
		}

		/**
		 * Create full report of expression processing.
		 */
		[[nodiscard]] static std::string create_summary() noexcept;
};

