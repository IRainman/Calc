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
		/**
		 * Create full report of expression processing.
		 */
		[[nodiscard]] constexpr static inline auto create_summary() noexcept
		{
			std::string ret;
			for (const auto& error : IssueManager::_errors)
			{
				ret += std::format("Error at pos {}: ", error.pos) + error.text + "\r\n";
			}
			return ret;
		}
};

