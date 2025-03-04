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
		[[nodiscard]] static std::string format(Value value) noexcept;
		/**
		 * Create full report of expression processing.
		 */
		[[nodiscard]] static std::string create_summary() noexcept;
};

