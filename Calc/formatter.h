#pragma once

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
		[[nodiscard]] static inline auto format(Value value) noexcept
		{
			return std::format("{}", value);
		}
#if 0
		template <typename... Args>
		[[nodiscard]] constexpr static inline auto format(const std::format_string<Args&&...> fmt, Args&&... args) noexcept
		{
			return std::format(fmt, args...);
		}
#endif
		/**
		 * Create full report of expression processing.
		 */
		[[nodiscard]] constexpr static inline auto create_summary() noexcept
		{
			std::string ret;
			for (auto& error : IssueManager::_errors)
			{
				ret += "Error at pos " + std::to_string(error.pos) + ": " + std::move(error.text) + "\r\n";
			}
			return ret;
		}
};

